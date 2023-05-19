#ifndef NETRIX_DS_H_
#define NETRIX_DS_H_

#include <stdbool.h>
#include <stdlib.h>

typedef struct netrix_deque_elem {
    void* elem;
    struct netrix_deque_elem* next;
    struct netrix_deque_elem* prev;
} netrix_deque_elem;

typedef struct netrix_deque {
    netrix_deque_elem* head;
    netrix_deque_elem* tail;
    int size;
} netrix_deque;

netrix_deque* netrix_create_deque(void);
void netrix_deque_push_front(netrix_deque*, void*);
void netrix_deque_push_back(netrix_deque*, void*);
void* netrix_deque_pop_front(netrix_deque*);
void* netrix_deque_pop_back(netrix_deque*);
void* netrix_deque_remove(netrix_deque*, int);
void netrix_deque_insert(netrix_deque*, void*, int);
int netrix_deque_size(netrix_deque*);
void* netrix_deque_get(netrix_deque*, int);
void netrix_free_deque(netrix_deque*);

netrix_deque_elem* netrix_deque_iterator(netrix_deque*);

typedef struct netrix_map_elem {
    const char* key;
    void* value;
} netrix_map_elem;

typedef struct netirx_map {
    netrix_deque* elems;
} netirx_map;

netirx_map* netrix_create_map(void);
void netrix_map_add(netirx_map*, const char*, void*);
void* netrix_map_remove(netirx_map*, const char*);
int netrix_map_exists_index(netirx_map*, const char*);
bool netrix_map_exists(netirx_map*, const char*);
void* netrix_map_get(netirx_map*, const char*);
int netrix_map_size(netirx_map*);
void netrix_free_map(netirx_map*);

netrix_deque_elem* map_iterator(netirx_map*);

typedef struct netrix_string {
  char *ptr;
  size_t len;
} netrix_string;

netrix_string* netrix_create_string(char*);
netrix_string* netrix_string_append(netrix_string*, char*);
netrix_string* netrix_string_appendn(netrix_string*, char*, size_t n);
char* netrix_string_str(netrix_string*);
size_t netrix_string_len(netrix_string*);
void netrix_free_string(netrix_string*);

#endif