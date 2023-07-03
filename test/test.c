#include "test.h"

int main(int argc, char *argv[]) {
    test_run(test_string_create);
    test_run(test_deque_create);
    test_run(test_deque_push_back);
    test_run(test_deque_fetch);
    test_run(test_deque_iter);
    test_run(test_map_exists_index);
    test_run(test_map_exists);
    test_run(test_map_get);

    return 0;
}