#include "ds.h"
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

netirx_map* netrix_create_map(void) {
    netirx_map* new_m = malloc(sizeof(netirx_map));
    new_m->elems = netrix_create_deque();

    return new_m;
}

void netrix_map_add(netirx_map* m, const char* key, void* value) {
    netrix_map_remove(m, key);

    netrix_map_elem* new = malloc(sizeof(netrix_map_elem));
    new->key = key;
    new->value = value;
    netrix_deque_push_back(m->elems, new);
}

void* netrix_map_remove(netirx_map* m, const char* key) {
    int index = netrix_map_exists_index(m,key);
    if (index == -1) {
        // Key does not exist
        return NULL;
    }

    netrix_map_elem* e = (netrix_map_elem*) netrix_deque_remove(m->elems, index);
    void* ret = e->value;
    free(e);
    return ret;
}

int netrix_map_exists_index(netirx_map* m, const char* key) {
    netrix_deque_elem* e = m->elems->head;
    for(int i=0; i< m->elems->size;i++) {
        netrix_map_elem* m_e = (netrix_map_elem*) e->elem;
        if(strcmp(m_e->key, key) == 0) {
            return i;
        }
        e = e->next;
    }
    return -1;
}

bool netrix_map_exists(netirx_map* m, const char* key) {
    return netrix_map_exists_index(m, key) == -1;
}

void* netrix_map_get(netirx_map* m, const char* key) {
    int index = netrix_map_exists_index(m, key);
    if (index == -1) {
        return NULL;
    }
    return netrix_deque_get(m->elems, index);
}

int netrix_map_size(netirx_map* m) {
    if(m == NULL) {
        return 0;
    }
    return netrix_deque_size(m->elems);
}

void netrix_free_map(netirx_map* m) {
    free(m);
}

netrix_deque_elem* map_iterator(netirx_map* m) {
    if(m == NULL) {
        return NULL;
    }
    return m->elems->head;
}


netrix_deque* netrix_create_deque(void) {
    netrix_deque* new_deque = malloc(sizeof(netrix_deque));
    new_deque->head = NULL;
    new_deque->tail = NULL;
    new_deque->size = 0;
    
    return new_deque;
}

void* netrix_deque_remove(netrix_deque* d, int pos) {
    if (pos >= d->size || pos < 0) {
        return NULL;
    }

    netrix_deque_elem* elem = d->head;
    for(int i = 0; i != pos; i++) {
        elem = elem->next;
    }

    netrix_deque_elem* prev = elem->prev;
    netrix_deque_elem* next = elem->next;
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

void netrix_deque_insert(netrix_deque* d, void* e, int pos) {
    if (pos < 0 || pos > d->size) {
        // Invalid position to insert in
        // will not do anything
        return;
    }
    netrix_deque_elem* new = malloc(sizeof(netrix_deque_elem));
    new->elem = e;
    new->prev = NULL;
    new->next = NULL;

    netrix_deque_elem* prev = d->head;
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

void netrix_deque_push_front(netrix_deque* d, void* elem) {
    netrix_deque_insert(d, elem, 0);
}

void netrix_deque_push_back(netrix_deque* d, void* elem) {
    netrix_deque_insert(d, elem, d->size);
}

void* netrix_deque_pop_front(netrix_deque* d) {
    return netrix_deque_remove(d, 0);
}

void* netrix_deque_pop_back(netrix_deque* d) {
    return netrix_deque_remove(d, d->size-1);
}

void* netrix_deque_get(netrix_deque* d, int pos) {
    if (pos >= d->size || pos < 0) {
        return NULL;
    }
    netrix_deque_elem* elem = d->head;
    for(int i = 0; i < pos; i++) {
        elem = elem->next;
    }
    return elem->elem;
}

int netrix_deque_size(netrix_deque* d) {
    if (d == NULL) {
        return 0;
    }
    return d->size;
}

void netrix_free_deque(netrix_deque* d) {
    free(d);
}

netrix_deque_elem* netrix_deque_iterator(netrix_deque* d) {
    if (d == NULL) {
        return NULL;
    }
    return d->head;
}


netrix_string* netrix_create_string(char* a) {
    netrix_string* s = malloc(sizeof(netrix_string));
    s->len = 0;
    s->ptr = malloc(s->len+1);
    s->ptr[s->len] = '\0';
    if (a != NULL && strlen(a) != 0) {
        s = netrix_string_append(s, a);
    }
    return s;
}

netrix_string* netrix_string_append(netrix_string* s, char* str) {
    if (s == NULL) {
        s = netrix_create_string(NULL);
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

netrix_string* netrix_string_appendn(netrix_string* s, char* str, size_t n) {
    char* s_cpy = malloc(n);
    strncpy(s_cpy, str, n);
    s = netrix_string_append(s, s_cpy);
    free(s_cpy);
    return s;
}

size_t netrix_string_len(netrix_string* s) {
    return s->len;
}

char* netrix_string_str(netrix_string* s) {
    char* resp = malloc(s->len+1);
    strncpy(resp, s->ptr, s->len);
    resp[s->len+1] = '\0';
    return resp;
}

void netrix_free_string(netrix_string* s) {
    free(s->ptr);
    free(s);
}