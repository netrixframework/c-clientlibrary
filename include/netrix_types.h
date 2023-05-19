#ifndef NETRIX_TYPES_H_
#define NETRIX_TYPES_H_

#include "ds.h"

typedef struct netrix_message {
    char* from;
    char* to;
    char* data;
    char* type;
    char* id;
} netrix_message;

typedef struct netrix_event {
    char* type;
    char* replica;
    netirx_map* params;
    long timestamp;
} netrix_event;

netrix_message* netrix_create_message(char*, char*, char*);
char* netrix_serialize_message(netrix_message*);
netrix_message* netrix_deserialize_message(char*);
void netrix_free_message(netrix_message*);

netrix_event* netrix_create_event(char*, netirx_map*, long);
char* netrix_serialize_event(netrix_event*);
void netrix_free_event(netrix_event*);

#endif