//////////////////////////////////////////////////////////////////////////////
// E.A.T. --- Eyeball Allocation Table (EAT), written by katahiromz.
// It's a specialized memory management system in C++. See file License.txt.
//////////////////////////////////////////////////////////////////////////////

#include "eat.hpp"

//////////////////////////////////////////////////////////////////////////////
// example and test

using namespace std;

template <typename T_SIZE, T_SIZE t_total_size>
void test1() {
    printf("## test1(%d,%d)\n", int(sizeof(T_SIZE)), int(t_total_size));

    EAT::MASTER<T_SIZE, t_total_size> master;
    typedef typename EAT::MASTER<T_SIZE, t_total_size>::entry_type entry_type;

    void *p1 = master.malloc(100);
    assert(p1 != NULL);
    assert(master._msize(p1) == 100);

    void *p2 = master.realloc(p1, 100);
    assert(p2 != NULL);
    assert(master._msize(p2) == 100);

    master.free(p2);
    master.compact();
    assert(master.empty());

    char *psz1 = master.strdup("ABC");
    assert(memcmp(psz1, "ABC", 3) == 0);

    T_SIZE offset = master.offset_from_ptr(psz1);
    assert(master.ptr_from_offset(offset) == psz1);

    char *psz2 = master.strdup(psz1);
    assert(memcmp(psz2, "ABC", 3) == 0);

    entry_type *entries = master.get_entries();
    const T_SIZE num = master.num_entries();
    for (T_SIZE i = 0; i < num; ++i) {
        puts(reinterpret_cast<char *>(master.ptr_from_offset(entries[i].m_offset)));
    }
}

template <typename T_SIZE, T_SIZE t_total_size>
void test2() {
    printf("## test2(%d,%d)\n", int(sizeof(T_SIZE)), int(t_total_size));

    char buf[t_total_size];
    EAT::MASTER<T_SIZE, t_total_size> *mas =
        EAT::eat_master<T_SIZE, t_total_size>(buf, true);
    //typedef typename EAT::MASTER<T_SIZE, t_total_size>::entry_type entry_type;

    void *p2 = mas->malloc(64);
    assert(p2 != NULL);
    memcpy(p2, "TEST", 4);

    void *p3 = mas->realloc(p2, 128);
    assert(memcmp(p3, "TEST", 4) == 0);
    assert(p3 != NULL);
    assert(mas->_msize(p3) == 128);
    assert(!mas->empty());

    mas->free(p3);
    mas->compact();
    assert(mas->empty());
}

bool print_ptr_fn(void *ptr) {
    puts(reinterpret_cast<char *>(ptr));
    return true;
}

template <typename T_SIZE, T_SIZE t_total_size>
void test3() {
    printf("## test3(%d,%d)\n", int(sizeof(T_SIZE)), int(t_total_size));

    EAT::MASTER<T_SIZE, t_total_size> master1;
    EAT::MASTER<T_SIZE, t_total_size> master2;
    typedef typename EAT::MASTER<T_SIZE, t_total_size>::entry_type entry_type;

    char *p1 = master1.strdup("ABC");
    char *p2 = master1.strdup("DEF");
    char *p3 = master1.strdup("GHI");
    assert(memcmp(p1, "ABC", 3) == 0);
    assert(memcmp(p2, "DEF", 3) == 0);
    assert(memcmp(p3, "GHI", 3) == 0);

    char *p4 = master2.strdup("JKL");
    char *p5 = master2.strdup("MNO");
    char *p6 = master2.strdup("PQR");
    assert(memcmp(p4, "JKL", 3) == 0);
    assert(memcmp(p5, "MNO", 3) == 0);
    assert(memcmp(p6, "PQR", 3) == 0);
    master2.free(p6);

    puts("master1");
    master1.foreach_valid_ptr(print_ptr_fn);
    puts("master2");
    master2.foreach_valid_ptr(print_ptr_fn);

    bool flag = master1.merge(master2);
    assert(flag);

    puts("master1");
    master1.foreach_valid_ptr(print_ptr_fn);
}

template <typename T_SIZE, T_SIZE t_total_size>
void test4() {
    printf("## test4(%d,%d)\n", int(sizeof(T_SIZE)), int(t_total_size));

    EAT::MASTER<T_SIZE, t_total_size> master;
    //typedef typename EAT::MASTER<T_SIZE, t_total_size>::entry_type entry_type;

    void *p1 = master.malloc(100);
    assert(p1 != NULL);
    assert(master._msize(p1) == 100);

    void *p2 = master.malloc(100);
    assert(p2 != NULL);
    assert(master._msize(p2) == 100);

    assert(master.valid_data_size() == 200);
    assert(master.num_entries() == 2);

    master.free(p2);
    assert(master.num_entries() == 1);
    assert(master.valid_data_size() == 100);
    master.free(p1);
    assert(master.num_entries() == 0);
    assert(master.valid_data_size() == 0);

    assert(master.empty());
}

int main(void) {
    test1<short, 300>();
    test1<long, 300>();
    test1<short, 400>();
    test1<long, 400>();

    test2<short, 300>();
    test2<long, 300>();
    test2<short, 400>();
    test2<long, 400>();

    test3<short, 300>();
    test3<long, 300>();
    test3<short, 400>();
    test3<long, 400>();

    test4<short, 300>();
    test4<long, 300>();
    test4<short, 400>();
    test4<long, 400>();

    return 0;
} // main

//////////////////////////////////////////////////////////////////////////////
