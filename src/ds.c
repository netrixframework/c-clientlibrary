#include "ds.h"
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

map* create_map(void) {
    map* new_m = malloc(sizeof(map));
    new_m->elems = create_deque();

    return new_m;
}

void map_add(map* m, const char* key, void* value) {
    map_remove(m, key);

    map_elem* new = malloc(sizeof(map_elem));
    new->key = key;
    new->value = value;
    deque_push_back(m->elems, new);
}

void* map_remove(map* m, const char* key) {
    int index = map_exists_index(m,key);
    if (index == -1) {
        // Key does not exist
        return NULL;
    }

    map_elem* e = (map_elem*) deque_remove(m->elems, index);
    void* ret = e->value;
    free(e);
    return ret;
}

int map_exists_index(map* m, const char* key) {
    deque_elem* e = m->elems->head;
    for(int i=0; i< m->elems->size;i++) {
        map_elem* m_e = (map_elem*) e->elem;
        if(strcmp(m_e->key, key) == 0) {
            return i;
        }
        e = e->next;
    }
    return -1;
}

bool map_exists(map* m, const char* key) {
    return map_exists_index(m, key) == -1;
}

void* map_get(map* m, const char* key) {
    int index = map_exists_index(m, key);
    if (index == -1) {
        return NULL;
    }
    return deque_get(m->elems, index);
}

int map_size(map* m) {
    return deque_size(m->elems);
}

void free_map(map* m) {
    free(m);
}

deque* create_deque(void) {
    deque* new_deque = malloc(sizeof(deque));
    new_deque->head = NULL;
    new_deque->tail = NULL;
    new_deque->size = 0;
    
    return new_deque;
}

void* deque_remove(deque* d, int pos) {
    if (pos >= d->size || pos < 0) {
        return NULL;
    }

    deque_elem* elem = d->head;
    for(int i = 0; i != pos; i++) {
        elem = elem->next;
    }

    deque_elem* prev = elem->prev;
    deque_elem* next = elem->next;
    if (prev == NULL) {
        d->head = next;
    } else if(next == NULL) {
        d->tail = prev;
    } else {
        prev->next = next;
        next->prev = prev;
    }
    void* e = elem->elem;
    free(elem);
    d->size--;
    return e;
}

void deque_insert(deque* d, void* e, int pos) {
    if (pos < 0 || pos > d->size) {
        // Invalid position to insert in
        // will not do anything
        return;
    }
    deque_elem* new = malloc(sizeof(deque_elem));
    new->elem = e;
    new->prev = NULL;
    new->next = NULL;

    deque_elem* prev = d->head;
    for(int i = 0; i < pos; i++) {
        prev = prev->next;
    }
    
    if (prev == NULL) {
        d->head = new;
        d->tail = new;
    } else {
        new->next = prev->next;
        prev->next = new;
        new->prev = prev;
        if(new->next == NULL) {
            d->tail = new;
        } else {
            new->next->prev = new;
        }
    }

    d->size++;
    return;
}

void deque_push_front(deque* d, void* elem) {
    deque_insert(d, elem, 0);
}

void deque_push_back(deque* d, void* elem) {
    deque_insert(d, elem, d->size);
}

void* deque_pop_front(deque* d) {
    return deque_remove(d, 0);
}

void* deque_pop_back(deque* d) {
    return deque_remove(d, d->size-1);
}

void* deque_get(deque* d, int pos) {
    if (pos >= d->size || pos < 0) {
        return NULL;
    }
    deque_elem* elem = d->head;
    for(int i = 0; i < pos; i++) {
        elem = elem->next;
    }
    return elem->elem;
}

void free_deque(deque* d) {
    free(d);
}