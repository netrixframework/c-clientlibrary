#include "ds.h"
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>

netrix_map* netrix_create_map(void) {
    netrix_map* new_m = malloc(sizeof(netrix_map));
    new_m->elems = netrix_create_deque();

    return new_m;
}

void netrix_map_add(netrix_map* m, const char* key, void* value) {
    netrix_map_remove(m, key);

    netrix_map_elem* new = malloc(sizeof(netrix_map_elem));
    new->key = key;
    new->value = value;
    netrix_deque_push_back(m->elems, new);
}

void* netrix_map_remove(netrix_map* m, const char* key) {
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

int netrix_map_exists_index(netrix_map* m, const char* key) {
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

bool netrix_map_exists(netrix_map* m, const char* key) {
    return netrix_map_exists_index(m, key) != -1;
}

void* netrix_map_get(netrix_map* m, const char* key) {
    int index = netrix_map_exists_index(m, key);
    if (index == -1) {
        return NULL;
    }
    netrix_map_elem* elem = (netrix_map_elem *) netrix_deque_get(m->elems, index);
    return elem->value;
}

int netrix_map_size(netrix_map* m) {
    if(m == NULL) {
        return 0;
    }
    return netrix_deque_size(m->elems);
}

void netrix_free_map(netrix_map* m) {
    free(m);
}

netrix_deque_elem* netrix_map_iterator(netrix_map* m) {
    if(m == NULL) {
        return NULL;
    }
    return m->elems->head;
}


netrix_deque* netrix_create_deque(void) {
    netrix_deque* new_deque = malloc(sizeof(netrix_deque));
    new_deque->head = NULL;
    new_deque->size = 0;

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    new_deque->mutex = mutex;
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
    } else {
        prev->next = next;
    }
    if (next != NULL) {
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

    if (pos == 0) {
        new->next = d->head;
        if (d->head != NULL) {
            d->head->prev = new;
        }
        d->head = new;
    } else if (pos == d->size) {
        netrix_deque_elem* last = d->head;
        while (last->next != NULL)
            last = last->next;

        last->next = new;
        new->prev = last;
    } else {
        netrix_deque_elem* prev = d->head;
        for(int i = 0; i < pos-1; i++) {
            prev = prev->next;
        }
        new->next = prev->next;
        prev->next = new;
        new->prev = prev;
        if (new->next != NULL) {
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
    pthread_mutex_destroy(&d->mutex);
    free(d);
}

int netrix_cdeque_insert(netrix_deque* d, void* elem, int pos) {
    if(pthread_mutex_lock(&d->mutex) != 0) {
        return -1;
    }
    netrix_deque_insert(d, elem, pos);
    return pthread_mutex_unlock(&d->mutex);
}

void* netrix_cdeque_remove(netrix_deque* d, int pos) {
    if(pthread_mutex_lock(&d->mutex) != 0) {
        return NULL;
    }
    void* ret = netrix_deque_remove(d, pos);
    pthread_mutex_unlock(&d->mutex);
    return ret;
}


int netrix_cdeque_push_front(netrix_deque* d, void* elem) {
    return netrix_cdeque_insert(d, elem, 0);
}

int netrix_cdeque_push_back(netrix_deque* d, void* elem) {
    return netrix_cdeque_insert(d, elem, d->size);
}

void* netrix_cdeque_pop_front(netrix_deque* d) {
    return netrix_cdeque_remove(d, 0);
}

void* netrix_cdeque_pop_back(netrix_deque* d) {
    return netrix_cdeque_remove(d, d->size-1);
}

void* netrix_cdeque_get(netrix_deque* d, int pos) {
    if(pthread_mutex_lock(&d->mutex) != 0) {
        return NULL;
    }
    void* ret = netrix_deque_get(d, pos);
    pthread_mutex_unlock(&d->mutex);
    return ret;
}

int netrix_cdeque_size(netrix_deque* d) {
    if(pthread_mutex_lock(&d->mutex) != 0) {
        return -1;
    }
    int size = netrix_deque_size(d);
    pthread_mutex_unlock(&d->mutex);
    return size;
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