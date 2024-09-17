// E.A.T. --- Eyeball Allocation Table (EAT), written by katahiromz.
// It's a specialized memory management system in C++. See file License.txt.
//////////////////////////////////////////////////////////////////////////////

#ifndef EYEBALL_ALLOCATION_TABLE
#define EYEBALL_ALLOCATION_TABLE    3  // Version 3

#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cassert>

namespace EAT
{
    //////////////////////////////////////////////////////////////////////////
    // EAT::ENTRY<T_SIZE> --- memory block info entry

    template <typename T_SIZE>
    struct ENTRY
    {
        // Types
        typedef T_SIZE                          size_type;
        typedef ENTRY<T_SIZE>                   self_type;

        enum FLAGS
        {
            FLAG_NONE = 0,
            FLAG_VALID = 1,
            FLAG_LOCKED = 2
        };

        // Members
        size_type           m_data_size;
        size_type           m_offset;
        size_type           m_flags;

        // Constructors
        ENTRY(size_type data_area_size, size_type offset)
            : m_data_size(data_area_size)
            , m_offset(offset)
            , m_flags(FLAG_VALID)
        {
        }
        ENTRY(size_type data_area_size, size_type offset, size_type flags)
            : m_data_size(data_area_size)
            , m_offset(offset)
            , m_flags(flags)
        {
        }

        // Attributes
        bool is_valid() const
        {
            return ((m_flags & FLAG_VALID) != 0);
        }
        void validate()
        {
            m_flags |= FLAG_VALID;
        }
        void invalidate()
        {
            m_flags &= ~FLAG_VALID;
        }
        bool is_locked() const
        {
            return ((m_flags & FLAG_LOCKED) != 0);
        }
        void lock(bool do_lock = true)
        {
            if (do_lock)
                m_flags |= FLAG_LOCKED;
            else
                m_flags &= ~FLAG_LOCKED;
        }
    }; // EAT::ENTRY<T_SIZE>

    //////////////////////////////////////////////////////////////////////////
    // EAT::HEAD<T_SIZE> --- the header data

    template <typename T_SIZE>
    struct HEAD
    {
        // Types
        typedef T_SIZE                          size_type;
        enum FLAGS
        {
            SIZE_TYPE_SIZE_MASK = 0x000000FFUL,
            FLAG_INVALID        = 0x00000100UL,
            FLAG_HIDDEN         = 0x00000200UL,
            FLAG_MOVEABLE       = 0x00000400UL,
            FLAG_PUBLIC         = 0x00000800UL,
            FLAG_CONFIDENTIAL   = 0x00001000UL,
            FLAG_ARCHIVE        = 0x00002000UL,
            FLAG_IMPORTANT      = 0x00004000UL,
            FLAG_SYSTEM         = 0x00008000UL,
            FLAG_UNCONFIRMED    = 0x00010000UL,
            FLAG_DRAFT          = 0x00020000UL,
            FLAG_FINAL          = 0x00040000UL,
            FLAG_RENEWAL        = 0x00080000UL,
            FLAG_EXPIRED        = 0x00100000UL,
            FLAG_ENCRYPTED      = 0x00200000UL,
            FLAG_INTERNAL       = 0x00400000UL,
            FLAG_EXTERNAL       = 0x00800000UL,
            FLAG_IMAGE          = 0x01000000UL,
            FLAG_PROGRAM_DATA   = 0x02000000UL,
            FLAG_MICROFILM      = 0x04000000UL,
            FLAG_REPORT         = 0x08000000UL,
            FLAG_LIST           = 0x10000000UL,
            FLAG_EVIDENCE       = 0x20000000UL,
            FLAG_AGREEMENT      = 0x40000000UL,
            FLAG_COMMUNICATION  = 0x80000000UL,
        };

        // Members
        char        m_magic[4];         // must be "EAT\0"
        uint32_t    m_flags;
        size_type   m_total_size;
        size_type   m_boudary_1;
        size_type   m_boudary_2;

        // Attributes
        bool is_valid() const
        {
            return ((memcmp(m_magic, "EAT\0", 4) == 0) &&
                    (size_type_size() == size_type(sizeof(size_type))) &&
                    (!(m_flags & FLAG_INVALID)) &&
                    (m_boudary_1 <= m_boudary_2) &&
                    (m_boudary_2 <= m_total_size));
        }
        void *get_body()
        {
            return this + 1;
        }
        const void *get_body() const
        {
            return this + 1;
        }
        size_type size_type_size() const
        {
            return size_type(m_flags & SIZE_TYPE_SIZE_MASK);
        }
        void modify_flags(uint32_t add_, uint32_t remove_)
        {
            m_flags &= ~remove_;
            m_flags |= add_;
        }
    }; // EAT::HEAD<T_SIZE>

    //////////////////////////////////////////////////////////////////////////
    // EAT::MASTER<T_SIZE> --- memory management master

    //////////////////////////////////////////////////////////////////////////
    //
    //             ---- "THE MASTER IMAGE" ----
    //
    //             +---------------------------+(top) == this
    //             |HEAD                       |
    //             +---------------------------+(head_size)
    //             |DATA #0 (variable length)  |
    //             |DATA #1                    |
    //             |  :                        |
    //             |  :     DATA_AREA          | | |
    //             |  :                        | | |
    //             |DATA #n-1 (grows downward) | V V
    //             +---------------------------+(boundary_1)
    //             |                           |
    //             |        FREE_AREA          |
    //             |                           |
    //             +---------------------------+(boundary_2)
    //             |ENTRY #n-1 (grows upward)  | A A
    //             |  :                        | | |
    //             |  :       TABLE            | | |
    //             |  :                        |
    //             |ENTRY #1                   |
    //             |ENTRY #0                   |
    //             +---------------------------+(bottom)
    //
    //////////////////////////////////////////////////////////////////////////

    template <typename T_SIZE>
    struct MASTER : protected HEAD<T_SIZE>
    {
        // Types
        typedef T_SIZE            size_type;
        typedef MASTER<T_SIZE>    self_type;
        typedef HEAD<T_SIZE>      head_type;
        typedef ENTRY<T_SIZE>     entry_type;

        // Constructors
        MASTER(size_type total_size)
        {
            init(total_size);
            assert(is_valid());
        }

        // Copy
        self_type& operator=(const MASTER<T_SIZE>& src)
        {
            copy(src);
            return *this;
        }
        bool copy(const MASTER<T_SIZE>& src)
        {
            assert(is_valid());
            assert(src.is_valid());

            if (this == &src)
                return true; // same

            if (total_size() == src.total_size()) // same total size
            {
                std::memcpy(this, &src, total_size());
                return true;
            }

            // different total size
            init(head_type::m_total_size);
            if (!merge(src))
                return false;

            assert(is_valid());
            assert(src.is_valid());
            return true;
        }

        // Merge
        bool merge(const MASTER<T_SIZE>& src)
        {
            assert(is_valid());
            assert(src.is_valid());

            if (this == &src)
                return true; // same

            // not same
            size_type addition = src.used_area_size() - src.head_size();
            assert(addition <= free_area_size());
            if (addition > free_area_size())
                return false; // not mergeable

            auto diff = size_type(head_type::m_boudary_1 - src.head_size());

            // add data
            auto data_size_2 = src.data_area_size();
            std::memcpy(get_free_area(), src.get_data_area(), data_size_2);
            head_type::m_boudary_1 += data_size_2;

            // add entries
            auto num = src.num_entries();
            auto entries1 = get_entries();
            entries1 -= num;
            auto entries2 = src.get_entries();
            for (size_type i = 0; i < num; ++i)
            {
                entries1[i].m_data_size = entries2[i].m_data_size;
                entries1[i].m_offset = size_type(entries2[i].m_offset + diff);
                entries1[i].m_flags = entries2[i].m_flags;
            }
            head_type::m_boudary_2 -= size_type(num * entry_size());

            assert(is_valid());
            assert(src.is_valid());
            return true;
        }

        // initialize
        void init(size_t total_size)
        {
            std::memcpy(head_type::m_magic, "EAT\0", 4);
            head_type::m_flags = size_type_size();
            head_type::m_total_size = total_size;
            head_type::m_boudary_1 = head_size();
            head_type::m_boudary_2 = total_size;
            assert(is_valid());
        }
        void clear(bool fill_by_zero = true)
        {
            assert(is_valid());
            head_type::m_boudary_1 = head_size();
            head_type::m_boudary_2 = head_type::m_total_size;
            if (fill_by_zero)
                std::memset(get_free_area(), 0, free_area_size());
            assert(is_valid());
        }

        // index access
        void *operator[](size_type index)
        {
            return reinterpret_cast<uint8_t *>(this)[index];
        }
        const void *operator[](size_type index) const
        {
            return reinterpret_cast<const uint8_t *>(this)[index];
        }
        size_type size() const
        {
            return head_type::m_total_size;
        }

        // Attributes
        bool is_valid() const
        {
            if (!head_type::is_valid())
                return false;

            return ((head_size() <= total_size()) &&
                    (total_size() == free_area_size() + used_area_size()) &&
                    (used_area_size() == head_size() + data_area_size() + table_size()) &&
                    ((table_size() % entry_size()) == 0));
        }
        bool empty() const
        {
            return (head_type::m_boudary_2 == head_type::m_total_size);
        }
        size_type head_size() const
        {
            return size_type(sizeof(head_type));
        }
        size_type size_type_size() const
        {
            return size_type(sizeof(size_type));
        }
        size_type total_size() const
        {
            return head_type::m_total_size;
        }
        size_type free_area_size() const
        {
            return size_type(head_type::m_boudary_2 - head_type::m_boudary_1);
        }
        size_type used_area_size() const
        {
            return size_type(head_type::m_boudary_1 + table_size());
        }
        size_type data_area_size() const
        {
            return size_type(head_type::m_boudary_1 - head_size());
        }
        size_type table_size() const
        {
            return size_type(head_type::m_total_size - head_type::m_boudary_2);
        }
        size_type entry_size() const
        {
            return size_type(sizeof(entry_type));
        }
        void *get_data_area()
        {
            return head_type::get_body();
        }
        const void *get_data_area() const
        {
            return head_type::get_body();
        }
        void *get_free_area()
        {
            return ptr_from_offset(head_type::m_boudary_1);
        }
        const void *get_free_area() const
        {
            return ptr_from_offset(head_type::m_boudary_1);
        }
        void modify_flags(uint32_t add_, uint32_t remove_)
        {
            modify_flags(add_, remove_);
        }

        // entries
        size_type num_entries() const
        {
            return size_type(table_size() / entry_size());
        }
        entry_type *get_entries()
        {
            char *p = reinterpret_cast<char *>(this);
            p += head_type::m_boudary_2;
            return reinterpret_cast<entry_type *>(p);
        }
        const entry_type *get_entries() const
        {
            const char *p = reinterpret_cast<const char *>(this);
            p += head_type::m_boudary_2;
            return reinterpret_cast<const entry_type *>(p);
        }

        // fetch the entry
        entry_type *fetch_entry(void *ptr)
        {
            return const_cast<entry_type *>(const_cast<const self_type*>(this)->fetch_entry(ptr));
        }
        const entry_type *fetch_entry(void *ptr) const
        {
            const entry_type *ret = NULL;
            assert(is_valid());
            if (!ptr)
                return NULL;

            auto offset = offset_from_ptr(ptr);
            // find entry of same offset
            auto entries = get_entries();
            for (size_type i = 0; i < num_entries(); ++i)
            {
                if (entries[i].m_offset == offset) // found
                {
                    ret = &entries[i];
                    break;
                }
            }
            assert(is_valid());
            return ret;
        }

        void free_entry(entry_type *entry)
        {
            assert(is_valid());
            if (!entry)
                return;

            entry->invalidate();

            auto entries = get_entries();
            if (entry != entries)
            {
                assert(is_valid());
                return;
            }

            // top entry
            auto num = num_entries();
            size_type i, data_deletion = 0;
            for (i = 0; i < num; ++i)
            {
                if (entries[i].is_valid())
                    break;
                data_deletion += entries[i].m_data_size;
            }

            // free invalids
            if (i == num)
            {
                clear();
            }
            else
            {
                head_type::m_boudary_1 -= data_deletion;
                head_type::m_boudary_2 += size_type(i * entry_size());
            }

            assert(is_valid());
        }

        // offsets and pointers
        size_type offset_from_ptr(const void *ptr) const
        {
            const char *this_p = reinterpret_cast<const char *>(this);
            const char *p = reinterpret_cast<const char *>(ptr);
            assert(this_p < p);
            return size_type(p - this_p);
        }
        void *ptr_from_offset(size_type offset)
        {
            char *this_p = reinterpret_cast<char *>(this);
            return this_p + offset;
        }
        const void *ptr_from_offset(size_type offset) const
        {
            const char *this_p = reinterpret_cast<const char *>(this);
            return this_p + offset;
        }

        // retrieve the size of memory
        size_type _msize_(void *ptr) const
        {
            assert(is_valid());
            auto entry = fetch_entry(ptr);
            if (!entry)
                return 0;
            return entry->m_data_size;
        }

        // allocate
        void *malloc_(size_type siz)
        {
            assert(is_valid());
            if (siz <= 0)
                return NULL;

            // size is non-zero
            auto required = size_type(siz + entry_size());
            if (required > free_area_size())
                return NULL; // out of memory

            // OK, allocatable
            auto offset = head_type::m_boudary_1;
            void *ret = reinterpret_cast<void *>(&reinterpret_cast<uint8_t *>(this)[offset]);
            head_type::m_boudary_1 += siz;
            head_type::m_boudary_2 -= entry_size();
            get_entries()[0] = entry_type(siz, offset);

            assert(is_valid());
            return ret;
        }

        void *calloc_(size_type nelem, size_type siz)
        {
            assert(is_valid());
            auto mult = nelem * siz;
            void *ret = malloc_(mult);
            if (!ret)
                return NULL;
            // fill by zero
            std::memset(ret, 0, mult);
            assert(is_valid());
            return ret;
        }

        // re-allocate
        void *realloc_(void *ptr, size_type siz)
        {
            assert(is_valid());
            if (ptr == NULL)
                return malloc_(siz);
            if (siz <= 0)
            {
                // size is zero
                free_(ptr);
                return NULL;
            }

            // find the entry
            auto entry = fetch_entry(ptr);
            assert(entry);
            if (!entry)
                return NULL; // entry not found

            // entry was found
            void *ret = malloc_(siz);
            if (!ret)
                return NULL;

            // copy contents
            if (siz <= entry->m_data_size)
                std::memcpy(ret, ptr, siz);
            else
                std::memcpy(ret, ptr, entry->m_data_size);

            // free old one
            free_entry(entry);

            assert(is_valid());
            return ret;
        }

        // free
        void free_(void * ptr)
        {
            assert(is_valid());
            if (!ptr)
                return;
            auto entry = fetch_entry(ptr);
            if (entry)
                free_entry(entry);
            assert(is_valid());
        }

        char *strdup_(const char *psz)
        {
            assert(is_valid());
            // calculate size
            auto len = size_type(strlen(psz));
            auto siz = size_type((len + 1) * sizeof(char));
            // allocate
            auto ret = reinterpret_cast<char *>(malloc_(siz));
            if (ret)
                std::memcpy(ret, psz, siz);
            assert(is_valid());
            return ret;
        }

        #ifdef _WIN32
            wchar_t *wcsdup_(const wchar_t *psz)
            {
                assert(is_valid());
                // calculate size
                auto len = size_type(wcslen(psz));
                auto siz = size_type((len + 1) * sizeof(wchar_t));
                // allocate
                auto ret = reinterpret_cast<wchar_t *>(malloc_(siz));
                if (ret)
                    std::memcpy(ret, psz, siz);
                assert(is_valid());
                return ret;
            }
        #endif

        void compact()
        {
            assert(is_valid());
            auto num = num_entries();
            if (num <= 0)
                return;

            // there are some entries
            auto entries = get_entries();
            auto offset = head_size();
            auto p = reinterpret_cast<char *>(get_data_area());

            // do scan the data area in reverse order
            auto ep = &entries[num]; // end of entries
            for (long i = long(num - 1); i >= 0; --i)
            {
                if (!entries[i].is_valid())
                    continue;

                // shift to p
                std::memmove(p, ptr_from_offset(entries[i].m_offset), entries[i].m_data_size);
                // fix offset
                entries[i].m_offset = offset;
                // copy entry and move up
                --ep;
                *ep = entries[i];
                // increase p and offset
                p += entries[i].m_data_size;
                offset += entries[i].m_data_size;
            }

            // update boundarys
            head_type::m_boudary_1 = offset;
            head_type::m_boudary_2 = offset_from_ptr(ep);

            assert(is_valid());
        }

        bool resize(size_type total)
        {
            assert(is_valid());
            auto num = num_entries();
            auto entries = get_entries();
            auto p = reinterpret_cast<char *>(entries);
            bool ret = false;
            if (head_type::m_total_size < total)
            {
                // move entries
                const auto diff = total - head_type::m_total_size;
                std::memmove(p + diff, p, num * entry_size());
                head_type::m_boudary_2 += diff;

                // fix total
                head_type::m_total_size = total;

                ret = true;
            }
            else if (head_type::m_total_size > total)
            {
                const auto diff = head_type::m_total_size - total;
                if (free_area_size() >= diff)
                {
                    // move entries
                    std::memmove(p - diff, p, num * entry_size());
                    head_type::m_boudary_2 -= diff;

                    // fix total
                    head_type::m_total_size = total;

                    ret = true;
                }
            }
            assert(is_valid());
            return ret;
        }

        // callback: bool T_ENTRY_FN(entry_type&);
        template <typename T_ENTRY_FN>
        void foreach_entry(T_ENTRY_FN& fn)
        {
            assert(is_valid());
            auto entries = get_entries();
            for (size_type i = 0; i < num_entries(); ++i)
            {
                auto& entry = entries[i];
                if (!entry.is_valid())
                    continue;
                if (!fn(entry))
                    break;
            }
            assert(is_valid());
        }

        // callback: bool T_PTR_FN(void *);
        template <typename T_PTR_FN>
        void foreach_ptr(T_PTR_FN& fn)
        {
            assert(is_valid());
            auto entries = get_entries();
            for (size_type i = 0; i < num_entries(); ++i)
            {
                auto& entry = entries[i];
                if (!entry.is_valid())
                    continue;
                void *ptr = ptr_from_offset(entry.m_offset);
                if (!fn(ptr))
                    break;
            }
            assert(is_valid());
        }
    }; // EAT::MASTER<T_SIZE>

    //////////////////////////////////////////////////////////////////////////////
    // EAT::create_master<T_SIZE>(total_size)
    // EAT::resize_master<T_SIZE>(old_master, new_total_size)
    // EAT::master_from_image<T_SIZE>(image_ptr, image_size = 0)
    // EAT::destroy_master

    template <typename T_SIZE>
    inline MASTER<T_SIZE> *create_master(size_t total_size)
    {
        auto master = reinterpret_cast<MASTER<T_SIZE> *>(std::malloc(total_size));
        if (!master)
            return NULL;
        master->init(total_size);
        return master;
    }

    inline void destroy_master(void *master)
    {
        std::free(master);
    }

    template <typename T_SIZE>
    inline MASTER<T_SIZE> *resize_master(MASTER<T_SIZE> *old_master, size_t new_total_size)
    {
        auto new_ptr = std::realloc(old_master, new_total_size);
        if (!new_ptr)
            return NULL;
        auto new_master = reinterpret_cast<MASTER<T_SIZE> *>(new_ptr);
        new_master->resize(new_total_size);
        return new_master;
    }

    template <typename T_SIZE>
    inline MASTER<T_SIZE> *master_from_image(void *image_ptr, size_t image_size = 0)
    {
        auto master = reinterpret_cast<MASTER<T_SIZE> *>(image_ptr);
        if (!master)
            return NULL;
        if (image_size)
            master->init(image_size);
        else
            master->init(master->total_size());
        return master;
    }
} // namespace EAT

#endif  // ndef EYEBALL_ALLOCATION_TABLE
