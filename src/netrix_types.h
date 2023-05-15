#ifndef NETRIX_TYPES_H_
#define NETRIX_TYPES_H_

#include "ds.h";

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
    map* params;
    long timestamp;
} netrix_event;

netrix_message* create_message(char*, char*, char*);
char* serialize_message(netrix_message*);
netrix_message* deserialize_message(char*);
void free_message(netrix_message*);

netrix_event* create_event(char*, map*, long);
char* serialize_event(netrix_event*);
void free_event(netrix_event*);

#endif