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
    if(m == NULL) {
        return 0;
    }
    return deque_size(m->elems);
}

void free_map(map* m) {
    free(m);
}

deque_elem* map_iterator(map* m) {
    if(m == NULL) {
        return NULL;
    }
    return m->elems->head;
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

int deque_size(deque* d) {
    if (d == NULL) {
        return 0;
    }
    return d->size;
}

void free_deque(deque* d) {
    free(d);
}

deque_elem* deque_iterator(deque* d) {
    if (d == NULL) {
        return NULL;
    }
    return d->head;
}


string* create_string(char* a) {
    string* s = malloc(sizeof(string));
    s->len = 0;
    s->ptr = malloc(s->len+1);
    s->ptr[s->len] = '\0';
    if (a != NULL && strlen(a) != 0) {
        s = string_append(s, a);
    }
    return s;
}

string* string_append(string* s, char* str) {
    if (s == NULL) {
        s = create_string(NULL);
    }
    if (str == NULL || strlen(str) == 0) {
        return s;
    }
    size_t new_len = s->len + strlen(str);
    s->ptr = realloc(s->ptr, new_len+1);
    memcpy(s->ptr+s->len, str, strlen(str));
    s->ptr[new_len] = '\0';
    s->len = new_len;
    return s;
}

string* string_appendn(string* s, char* str, size_t n) {
    char* s_cpy = malloc(n);
    strncpy(s_cpy, str, n);
    s = string_append(s, s_cpy);
    free(s_cpy);
    return s;
}

size_t string_len(string* s) {
    return s->len;
}

char* string_str(string* s) {
    char* resp = malloc(s->len+1);
    strncpy(resp, s->ptr, s->len);
    resp[s->len+1] = '\0';
    return resp;
}

void free_string(string* s) {
    free(s->ptr);
    free(s);
}