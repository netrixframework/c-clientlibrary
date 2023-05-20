#ifndef NETRIX_DS_H_
#define NETRIX_DS_H_

#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>

// Generic Deque element that stores elements as void*
typedef struct netrix_deque_elem {
    void* elem;
    struct netrix_deque_elem* next;
    struct netrix_deque_elem* prev;
} netrix_deque_elem;

// Generic doubly linked list
// Includes mutex to enable thread safe operations
typedef struct netrix_deque {
    netrix_deque_elem* head;
    netrix_deque_elem* tail;
    int size;
    pthread_mutex_t mutex;
} netrix_deque;

// Creation and destruction of deque
netrix_deque* netrix_create_deque(void);
void netrix_free_deque(netrix_deque*);

// Deque operations
void netrix_deque_push_front(netrix_deque*, void*);
void netrix_deque_push_back(netrix_deque*, void*);
void* netrix_deque_pop_front(netrix_deque*);
void* netrix_deque_pop_back(netrix_deque*);
void* netrix_deque_remove(netrix_deque*, int);
void netrix_deque_insert(netrix_deque*, void*, int);
int netrix_deque_size(netrix_deque*);
void* netrix_deque_get(netrix_deque*, int);

// Thread safe deque operations
int netrix_cdeque_insert(netrix_deque*, void*, int);
void* netrix_cdeque_remove(netrix_deque*, int);
int netrix_cdeque_push_front(netrix_deque*, void*);
int netrix_cdeque_push_back(netrix_deque*, void*);
void* netrix_cdeque_pop_front(netrix_deque*);
void* netrix_cdeque_pop_back(netrix_deque*);
void* netrix_cdeque_get(netrix_deque*, int);
int netrix_cdeque_size(netrix_deque*);

// Deque iteration
netrix_deque_elem* netrix_deque_iterator(netrix_deque*);

typedef struct netrix_map_elem {
    const char* key;
    void* value;
} netrix_map_elem;

typedef struct netrix_map {
    netrix_deque* elems;
} netrix_map;

netrix_map* netrix_create_map(void);
void netrix_map_add(netrix_map*, const char*, void*);
void* netrix_map_remove(netrix_map*, const char*);
int netrix_map_exists_index(netrix_map*, const char*);
bool netrix_map_exists(netrix_map*, const char*);
void* netrix_map_get(netrix_map*, const char*);
int netrix_map_size(netrix_map*);
void netrix_free_map(netrix_map*);

netrix_deque_elem* map_iterator(netrix_map*);

// Netrix string operations
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