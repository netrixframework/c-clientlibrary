#ifndef NETRIX_DS_H_
#define NETRIX_DS_H_

#include <stdbool.h>

typedef struct map_elem {
    const char* key;
    void* value;
} map_elem;

typedef struct map {
    deque* elems;
} map;

map* create_map(void);
void map_add(map*, const char*, void*);
void* map_remove(map*, const char*);
int map_exists_index(map*, const char*);
bool map_exists(map*, const char*);
void* map_get(map*, const char*);
int map_size(map*);
void free_map(map*);

typedef struct deque_elem {
    void* elem;
    deque_elem* next;
    deque_elem* prev;
} deque_elem;

typedef struct deque {
    deque_elem* head;
    deque_elem* tail;
    int size;
} deque;

deque* create_deque(void);
void deque_push_front(deque*, void*);
void deque_push_back(deque*, void*);
void* deque_pop_front(deque*);
void* deque_pop_back(deque*);
void* deque_remove(deque*, int);
void deque_insert(deque*, void*, int);
int deque_size(deque*);
void* deque_get(deque*, int);
void free_deque(deque*);

#endif