// E.A.T. --- Eyeball Allocation Table (EAT), written by katahiromz.
// It's a specialized memory management system in C++. See file License.txt.
//////////////////////////////////////////////////////////////////////////////

#include "eat.h"

template <typename T_SIZE, T_SIZE t_total_size>
void test1(void)
{
    printf("## test1(%d,%d)\n", int(sizeof(T_SIZE)), int(t_total_size));

    auto master = EAT::create_master<T_SIZE>(t_total_size);
    typedef typename EAT::MASTER<T_SIZE>::entry_type entry_type;

    void *p1 = master->malloc_(100);
    assert(p1 != NULL);
    assert(master->_msize_(p1) == 100);

    void *p2 = master->realloc_(p1, 100);
    assert(p2 != NULL);
    assert(master->_msize_(p2) == 100);

    master->free_(p2);
    master->compact();
    assert(master->empty());

    char *psz1 = master->strdup_("ABC");
    assert(memcmp(psz1, "ABC", 3) == 0);

    T_SIZE offset = master->offset_from_ptr(psz1);
    assert(master->ptr_from_offset(offset) == psz1);

    char *psz2 = master->strdup_(psz1);
    assert(memcmp(psz2, "ABC", 3) == 0);

    auto entries = master->get_entries();
    auto num = master->num_entries();
    for (T_SIZE i = 0; i < num; ++i)
    {
        puts(reinterpret_cast<char *>(master->ptr_from_offset(entries[i].m_offset)));
    }

    EAT::destroy_master(master);
}

int main(void)
{
    assert(sizeof(int8_t) == 1);
    assert(sizeof(int16_t) == 2);
    assert(sizeof(int32_t) == 4);
    assert(sizeof(uint8_t) == 1);
    assert(sizeof(uint16_t) == 2);
    assert(sizeof(uint32_t) == 4);

    test1<uint16_t, 300>();
    test1<uint32_t, 300>();
    test1<uint16_t, 400>();
    test1<uint32_t, 400>();

    return 0;
}
