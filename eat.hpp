//////////////////////////////////////////////////////////////////////////////
// E.A.T. --- Eyeball Allocation Table (EAT), written by katahiromz.
// It's a specialized memory management system in C++. See file License.txt.
//////////////////////////////////////////////////////////////////////////////

#ifndef EYEBALL_ALLOCATION_TABLE
#define EYEBALL_ALLOCATION_TABLE    0   // Version 0

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>

//////////////////////////////////////////////////////////////////////////////
// NOTE: You can pre-#define the eat_status_dirty and eat_status_bad macros.

#ifndef eat_status_dirty
    #define eat_status_dirty(pMaster)   assert(0)
#endif

#ifndef eat_status_bad
    #define eat_status_bad(pMaster)     assert(0)
#endif

//////////////////////////////////////////////////////////////////////////////

namespace EAT
{
    //////////////////////////////////////////////////////////////////////////
    // EAT::ENTRY<T_SIZE> --- memory block info entry

    template <typename T_SIZE>
    struct ENTRY {
        // Types
        typedef T_SIZE                          size_type;
        typedef ENTRY<T_SIZE>                   self_type;

        enum FLAGS {
            FLAG_NONE = 0,
            FLAG_VALID = 1,
            FLAG_LOCKED = 2
        };

        // Members
        size_type           m_data_size;
        size_type           m_offset;
        size_type           m_flags;

        // Constructors
        ENTRY(size_type data_area_size, size_type offset) :
            m_data_size(data_area_size), m_offset(offset),
            m_flags(FLAG_VALID) { }
        ENTRY(size_type data_area_size, size_type offset, size_type flags) :
            m_data_size(data_area_size), m_offset(offset), m_flags(flags) { }

        // Attributes
        bool is_valid() const {
            return ((m_flags & FLAG_VALID) != 0);
        }
        void validate() {
            m_flags |= FLAG_VALID;
        }
        void invalidate() {
            m_flags &= ~FLAG_VALID;
        }
        bool is_locked() const {
            return ((m_flags & FLAG_LOCKED) != 0);
        }
    }; // EAT::ENTRY<T_SIZE>

    //////////////////////////////////////////////////////////////////////////
    // EAT::HEAD<T_SIZE> --- the header data

    template <typename T_SIZE>
    struct HEAD {
        // Types
        typedef T_SIZE                          size_type;

        // Members
        size_type m_total_size;
        size_type m_boudary_1;
        size_type m_boudary_2;

        // Attributes
        bool is_valid() const {
            bool ret;
            if (m_boudary_2 < m_boudary_1) {
                ret = false;
            } else if (m_total_size < m_boudary_2) {
                ret = false;
            } else {
                ret = true;
            }
            return ret;
        }
        void *get_body() {
            return this + 1;
        }
        const void *get_body() const {
            return this + 1;
        }
    }; // EAT::HEAD<T_SIZE>

    //////////////////////////////////////////////////////////////////////////
    // EAT::MASTER<T_SIZE, t_total_size> --- memory management master

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

    template <typename T_SIZE, T_SIZE t_total_size>
    struct MASTER {
        // Types
        typedef T_SIZE                          size_type;
        typedef MASTER<T_SIZE, t_total_size>    self_type;
        typedef HEAD<T_SIZE>                    head_type;
        typedef ENTRY<T_SIZE>                   entry_type;
        union {
            unsigned char   m_space[t_total_size];
            head_type       m_head;
        };

        // Constructors
        MASTER() {
            clear();
            assert(is_valid());
        }
        template <T_SIZE t_total_size_2>
        MASTER(const MASTER<T_SIZE, t_total_size_2>& src) {
            clear();
            merge<t_total_size_2>(src);
            assert(is_valid());
        }

        // Copy
        template <T_SIZE t_total_size_2>
        self_type& operator=(const MASTER<T_SIZE, t_total_size_2>& src) {
            assert(is_valid());
            copy<t_total_size_2>(src);
            assert(is_valid());
            return *this;
        }
        template <T_SIZE t_total_size_2>
        bool copy(const MASTER<T_SIZE, t_total_size_2>& src) {
            bool ret = false;
            assert(is_valid());
            assert(src.is_valid());
            if (this != &src) {
                // not same
                const size_type total = total_size();
                if (total >= src.total_size()) {
                    // same total size
                    using namespace std;
                    memcpy(this, &src, total);
                    ret = true;
                } else {
                    // different total size
                    clear();
                    ret = merge<t_total_size_2>(src);
                }
            } else {
                // same
                ret = true;
            }
            assert(is_valid());
            assert(src.is_valid());
            return ret;
        }

        // Merge
        template <T_SIZE t_total_size_2>
        bool merge(const MASTER<T_SIZE, t_total_size_2>& src) {
            bool ret = false;
            assert(is_valid());
            assert(src.is_valid());
            if (this != &src) {
                // not same
                size_type addition = src.used_area_size() - src.head_size();
                if (addition <= free_area_size()) {
                    using namespace std;
                    // it's mergeable
                    ret = true;
                    const size_type diff =
                        size_type(m_head.m_boudary_1 - src.head_size());

                    // add data
                    const size_type data_size_2 = src.data_area_size();
                    memcpy(get_free_area(), src.get_data_area(), data_size_2);
                    m_head.m_boudary_1 += data_size_2;

                    // add entries
                    const size_type num = src.num_entries();
                    entry_type *entries1 = get_entries();
                    entries1 -= num;
                    const entry_type *entries2 = src.get_entries();
                    for (size_type i = 0; i < num; ++i) {
                        entries1[i].m_data_size = entries2[i].m_data_size;
                        entries1[i].m_offset = size_type(entries2[i].m_offset + diff);
                        entries1[i].m_flags = entries2[i].m_flags;
                    }
                    m_head.m_boudary_2 -= size_type(num * entry_size());
                }
            }
            assert(is_valid());
            assert(src.is_valid());
            return ret;
        }

        // initialize
        void clear() {
            m_head.m_total_size = t_total_size;
            m_head.m_boudary_1 = head_size();
            m_head.m_boudary_2 = t_total_size;
            assert(is_valid());
        }

        // Attributes
        bool is_valid() const {
            bool ret = m_head.is_valid();
            if (ret) {
                const size_type tos = total_size();
                const size_type hs = head_size();
                const size_type das = data_area_size();
                const size_type fas = free_area_size();
                if (m_head.m_total_size > t_total_size) {
                    assert(0);
                    ret = false;
                } else if (hs > tos) {
                    // head size must be greater than total size
                    assert(0);
                    ret = false;
                } else if (tos != (fas + used_area_size())) {
                    // total size must be free area size + used area size
                    assert(0);
                    ret = false;
                } else if (das != (valid_data_size() + invalid_data_size())) {
                    // data area size must be sum of valid size and invalid size
                    assert(0);
                    ret = false;
                } else if (used_area_size() != (hs + das + table_size())) {
                    // used area size must be head size + data area size + table size
                    assert(0);
                    ret = false;
                } else if ((table_size() % entry_size()) != 0) {
                    // table size must be a multiple of entry size
                    assert(0);
                    ret = false;
                } else {
                    // check data sizes
                    const size_type num = num_entries();
                    const entry_type *entries = get_entries();
                    for (size_type i = 0; i < num; ++i) {
                        if (entries[i].m_data_size == 0) {
                            // data size must be non-zero
                            ret = false;
                            assert(0);
                            break;
                        }
                    }
                }
                if (ret) {
                    // check offset order
                    const size_type num = num_entries();
                    const entry_type *entries = get_entries();
                    for (size_type i = 1; i < num; ++i) {
                        if (entries[i - 1].m_offset < entries[i].m_offset) {
                            ret = false;
                            assert(0);
                            break;
                        }
                    }
                }
            }
            return ret;
        }
        bool empty() const {
            return (m_head.m_boudary_2 == m_head.m_total_size);
        }
        size_type head_size() const {
            return size_type(sizeof(head_type));
        }
        size_type total_size() const {
            return m_head.m_total_size;
        }
        size_type free_area_size() const {
            return size_type(m_head.m_boudary_2 - m_head.m_boudary_1);
        }
        size_type used_area_size() const {
            return size_type(m_head.m_boudary_1 + table_size());
        }
        size_type data_area_size() const {
            return size_type(m_head.m_boudary_1 - head_size());
        }
        size_type valid_data_size() const {
            size_type siz = 0;
            const size_type num = num_entries();
            const entry_type *entries = get_entries();
            for (size_type i = 0; i < num; ++i) {
                if (entries[i].is_valid()) {
                    siz += entries[i].m_data_size;
                }
            }
            return siz;
        }
        size_type invalid_data_size() const {
            size_type siz = 0;
            const size_type num = num_entries();
            const entry_type *entries = get_entries();
            for (size_type i = 0; i < num; ++i) {
                if (!entries[i].is_valid()) {
                    siz += entries[i].m_data_size;
                }
            }
            return siz;
        }
        size_type table_size() const {
            return size_type(m_head.m_total_size - m_head.m_boudary_2);
        }
        size_type entry_size() const {
            return size_type(sizeof(entry_type));
        }
        void *get_data_area() {
            return m_head.get_body();
        }
        const void *get_data_area() const {
            return m_head.get_body();
        }
        void *get_free_area() {
            return ptr_from_offset(m_head.m_boudary_1);
        }
        const void *get_free_area() const {
            return ptr_from_offset(m_head.m_boudary_1);
        }

        // entries
        size_type num_entries() const {
            return size_type(table_size() / entry_size());
        }
        entry_type *get_entries() {
            char *p = reinterpret_cast<char *>(this);
            p += m_head.m_boudary_2;
            return reinterpret_cast<entry_type *>(p);
        }
        const entry_type *get_entries() const {
            const char *p = reinterpret_cast<const char *>(this);
            p += m_head.m_boudary_2;
            return reinterpret_cast<const entry_type *>(p);
        }

        // index access
        unsigned char& operator[](size_type offset) {
            assert(is_valid());
            assert(offset <= t_total_size);
            return m_space[offset];
        }
        const unsigned char& operator[](size_type offset) const {
            assert(is_valid());
            assert(offset <= t_total_size);
            return m_space[offset];
        }

        // fetch the entry
        entry_type *fetch_entry(void *ptr) {
            entry_type *ret = NULL;
            assert(is_valid());
            assert(ptr != NULL);
            if (ptr != NULL) {
                const size_type offset = offset_from_ptr(ptr);
                // find entry of same offset
                entry_type *entries = get_entries();
                const size_type num = num_entries();
                for (size_type i = 0; i < num; ++i) {
                    if (entries[i].m_offset == offset) {
                        // found
                        ret = &entries[i];
                        break;
                    }
                }
                if (ret == NULL) {
                    eat_status_dirty(this);
                }
            }
            assert(is_valid());
            return ret;
        } // fetch
        const entry_type *fetch_entry(void *ptr) const {
            const entry_type *ret = NULL;
            assert(is_valid());
            assert(ptr != NULL);
            if (ptr != NULL) {
                const size_type offset = offset_from_ptr(ptr);
                // find entry of same offset
                const entry_type *entries = get_entries();
                const size_type num = num_entries();
                for (size_type i = 0; i < num; ++i) {
                    if (entries[i].m_offset == offset) {
                        // found
                        ret = &entries[i];
                        break;
                    }
                }
                if (ret == NULL) {
                    eat_status_dirty(this);
                }
            }
            assert(is_valid());
            return ret;
        } // fetch

        void free_entry(entry_type *entry) {
            assert(is_valid());
            if (entry != NULL) {
                entry->invalidate();

                entry_type *entries = get_entries();
                if (entry == entries) {
                    // top entry
                    const size_type num = num_entries();
                    size_type i, data_deletion = 0;
                    for (i = 0; i < num; ++i) {
                        if (entries[i].is_valid()) {
                            break;
                        }
                        data_deletion += entries[i].m_data_size;
                    }
                    // free invalids
                    if (i == num) {
                        clear();
                    } else {
                        m_head.m_boudary_1 -= data_deletion;
                        m_head.m_boudary_2 += size_type(i * entry_size());
                    }
                }
            }
            assert(is_valid());
        }

        // offsets and pointers
        size_type offset_from_ptr(const void *ptr) const {
            const char *this_p = reinterpret_cast<const char *>(this);
            const char *p = reinterpret_cast<const char *>(ptr);
            assert(this_p < p);
            return size_type(p - this_p);
        }
        void *ptr_from_offset(size_type offset) {
            char *this_p = reinterpret_cast<char *>(this);
            return this_p + offset;
        }
        const void *ptr_from_offset(size_type offset) const {
            const char *this_p = reinterpret_cast<const char *>(this);
            return this_p + offset;
        }

        // retrieve the size of memory
        size_type _msize(void *ptr) const {
            assert(is_valid());
            size_type ret = 0;
            const entry_type *entry = fetch_entry(ptr);
            if (entry != NULL) {
                // entry was found
                ret = entry->m_data_size;
            }
            return ret;
        }

        // allocate
        void *malloc(size_type siz) {
            void *ret;
            assert(is_valid());
            if (siz == 0) {
                // size is zero
                ret = NULL;
            } else {
                // size is non-zero
                size_type required = size_type(siz + entry_size());
                if (required > free_area_size()) {
                    // out of memory
                    eat_status_bad(this);
                    ret = NULL;
                } else {
                    // OK, allocatable
                    const size_type offset = m_head.m_boudary_1;
                    ret = reinterpret_cast<void *>(&m_space[offset]);
                    m_head.m_boudary_1 += siz;
                    m_head.m_boudary_2 -= entry_size();
                    get_entries()[0] = entry_type(siz, offset);
                }
            }
            assert(is_valid());
            return ret;
        } // malloc

        void *calloc(size_type nelem, size_type siz) {
            assert(is_valid());
            // allocate
            size_type mult = nelem * siz;
            void *ret = malloc(mult);
            if (ret != NULL) {
                // fill by zero
                using namespace std;
                memset(ret, 0, mult);
            }
            assert(is_valid());
            return ret;
        }

        // re-allocate
        void *realloc(void *ptr, size_type siz) {
            void *ret;
            assert(is_valid());
            if (ptr == NULL) {
                // pointer is NULL
                ret = malloc(siz);
            } else if (siz == 0) {
                // size is zero
                free(ptr);
                ret = NULL;
            } else {
                // find the entry
                entry_type *entry = fetch_entry(ptr);
                assert(entry != NULL);
                if (entry == NULL) {
                    // entry not found
                    ret = NULL;
                } else {
                    // entry was found
                    ret = malloc(siz);
                    if (ret != NULL) {
                        // copy contents
                        using namespace std;
                        if (siz <= entry->m_data_size) {
                            memcpy(ret, ptr, siz);
                        } else {
                            memcpy(ret, ptr, entry->m_data_size);
                        }
                        // free old one
                        free_entry(entry);
                    }
                }
            }
            assert(is_valid());
            return ret;
        } // realloc

        // free
        void free(void * ptr) {
            assert(is_valid());
            if (ptr != NULL) {
                entry_type *entry = fetch_entry(ptr);
                if (entry) {
                    // entry was found
                    free_entry(entry);
                }
            }
            assert(is_valid());
        } // free

        char *strdup(const char *psz) {
            assert(is_valid());
            // calculate size
            using namespace std;
            size_type len = size_type(strlen(psz));
            size_type siz = size_type((len + 1) * sizeof(char));
            // allocate
            char *ret = reinterpret_cast<char *>(malloc(siz));
            if (ret != NULL) {
                // copy contents
                memcpy(ret, psz, siz);
            }
            assert(is_valid());
            return ret;
        }

        #ifdef _WIN32
            wchar_t *wcsdup(const wchar_t *psz) {
                assert(is_valid());
                // calculate size
                using namespace std;
                size_type len = size_type(wcslen(psz));
                size_type siz = size_type((len + 1) * sizeof(wchar_t));
                // allocate
                wchar_t *ret = reinterpret_cast<wchar_t *>(malloc(siz));
                if (ret != NULL) {
                    // copy contents
                    memcpy(ret, psz, siz);
                }
                assert(is_valid());
                return ret;
            }
        #endif

        void compact() {
            assert(is_valid());
            const size_type num = num_entries();
            if (num > 0) {
                // there are some entries
                entry_type *entries = get_entries();
                size_type offset = head_size();
                char *p = reinterpret_cast<char *>(get_data_area());

                // do scan the data area in reverse order
                entry_type *ep = &entries[num]; // end of entries
                for (int i = int(num - 1); i >= 0; --i) {
                    if (entries[i].is_valid()) {
                        // this entry is valid
                        // shift to p
                        using namespace std;
                        memmove(p, ptr_from_offset(entries[i].m_offset),
                                entries[i].m_data_size);
                        // fix offset
                        entries[i].m_offset = offset;
                        // copy entry and move up
                        --ep;
                        *ep = entries[i];
                        // increase p and offset
                        p += entries[i].m_data_size;
                        offset += entries[i].m_data_size;
                    }
                }
                // update boundarys
                m_head.m_boudary_1 = offset;
                m_head.m_boudary_2 = offset_from_ptr(ep);
            }
            assert(is_valid());
        } // compact

        bool resize_total(size_type total = t_total_size) {
            bool ret = false;
            assert(is_valid());
            assert(total <= t_total_size);
            if (total <= t_total_size) {
                const size_type num = num_entries();
                entry_type *entries = get_entries();
                char *p = reinterpret_cast<char *>(entries);
                using namespace std;
                if (m_head.m_total_size < total) {
                    const size_type diff = total - m_head.m_total_size;
                    // move entries
                    memmove(p + diff, p, num * entry_size());
                    // fix total
                    m_head.m_total_size = total;
                } else if (m_head.m_total_size > total) {
                    const size_type diff = m_head.m_total_size - total;
                    // move entries
                    memmove(p, p + diff, num * entry_size());
                    // fix total
                    m_head.m_total_size = total;
                }
                ret = true;
            }
            assert(is_valid());
            return ret;
        } // resize_total

        // file operations
        bool load_from_file(const char *file_name) {
            assert(is_valid());
            using namespace std;
            bool ret = false;
            // open file
            FILE *fp = fopen(file_name, "rb");
            assert(fp != NULL);
            if (fp != NULL) {
                // read from file
                ret = (fread(this, sizeof(self_type), 1, fp) == 1);
                assert(ret);
                if (ret) {
                    // is valid?
                    ret = is_valid();
                    if (ret) {
                        resize_total();
                    } else {
                        clear();
                    }
                    assert(ret);
                }
                // close file
                fclose(fp);
            }
            assert(is_valid());
            return ret;
        } // load_from_file
        bool save_to_file(const char *file_name) const {
            assert(is_valid());
            using namespace std;
            bool ret = false;
            // create file
            FILE *fp = fopen(file_name, "wb");
            assert(fp != NULL);
            if (fp != NULL) {
                // write to file
                ret = (fwrite(this, sizeof(self_type), 1, fp) == 1);
                assert(ret);
                // close file
                fclose(fp);
                if (!ret) {
                    // if not valid, delete it
                    unlink(file_name);
                }
            }
            assert(is_valid());
            return ret;
        } // save_to_file
    }; // EAT::MASTER<T_SIZE, t_total_size>

    //////////////////////////////////////////////////////////////////////////
    // MASTER *eat_master<...>(void *data_block, bool do_init = false);

    template <typename T_SIZE, T_SIZE t_total_size>
    inline
    MASTER<T_SIZE, t_total_size> *
    eat_master(void *data_block, bool do_init = false) {
        assert(data_block != NULL);
        MASTER<T_SIZE, t_total_size> *mas =
            reinterpret_cast<MASTER<T_SIZE, t_total_size> *>(data_block);
        if (do_init) {
            mas->clear();
        }
        assert(mas->is_valid());
        return mas;
    } // eat_master

    template <typename T_SIZE, T_SIZE t_total_size>
    inline
    const MASTER<T_SIZE, t_total_size> *
    eat_master(const void *data_block) {
        assert(data_block != NULL);
        const MASTER<T_SIZE, t_total_size> *mas =
            reinterpret_cast<
                const MASTER<T_SIZE, t_total_size> *>(data_block);
        assert(mas->is_valid());
        return mas;
    } // eat_master

} // namespace EAT

#endif  // ndef EYEBALL_ALLOCATION_TABLE

//////////////////////////////////////////////////////////////////////////////
