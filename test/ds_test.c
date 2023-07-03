#include "netrix.h"
#include "test.h"

#include <assert.h>
#include <string.h>
#include <stdbool.h>

void test_string_create() {
    netrix_string* s = netrix_create_string("hello");
    char *out = netrix_string_str(s);

    int check = strcmp("hello", out);
    assert(check == 0);

    netrix_free_string(s);
}

void test_map_exists_index() {
    netrix_map* m = netrix_create_map();
    netrix_map_add(m, "test", "test");
    int index = netrix_map_exists_index(m, "test");

    assert(index == 0);

    netrix_free_map(m);
}

void test_map_exists() {
    netrix_map* m = netrix_create_map();
    bool exists = netrix_map_exists(m, "test");

    assert(exists == 0);

    netrix_free_map(m);
}

void test_map_get() {
    netrix_map* m = netrix_create_map();
    netrix_map_add(m, "test", "test");

    char *val = netrix_map_get(m, "test");

    assert(strcmp(val, "test") == 0);

    netrix_free_map(m);
}

void test_deque_create() {
    netrix_deque* d = netrix_create_deque();

    assert(netrix_deque_size(d) == 0);

    netrix_free_deque(d);
}

void test_deque_push_back() {
    netrix_deque* d = netrix_create_deque();
    netrix_deque_push_back(d, 1);
    netrix_deque_push_back(d, 2);

    assert(netrix_deque_size(d) == 2);

    netrix_free_deque(d);
}

void test_deque_fetch() {
    netrix_deque* d = netrix_create_deque();
    netrix_deque_push_back(d, 1);

    assert(netrix_deque_size(d) == 1);

    netrix_deque_elem* e = d->head;
    int val = (int) e->elem;

    assert(val == 1);

    netrix_free_deque(d);
}

void test_deque_iter() {
    int elems[3] = {1,2,3};

    netrix_deque* d= netrix_create_deque();
    netrix_deque_push_back(d, 1);
    netrix_deque_push_back(d, 2);
    netrix_deque_push_back(d, 3);

    netrix_deque_elem* e = d->head;
    for(int i=0; i < d->size;i++) {
        assert(e != NULL);
        int m_e = (int) e->elem;
        assert(m_e == elems[i]);
        e = e->next;
    }

    netrix_free_deque(d);
}