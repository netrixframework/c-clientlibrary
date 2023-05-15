
#ifndef NETRIX_CLIENT_H_
#define NETRIX_CLIENT_H_

#include "netrix_types.h"
#include "ds.h"
#include <stdbool.h>

typedef void (*directive_handler)(char*);

typedef struct client_config {
    char* id;
    char* netrix_addr;
    char* listen_addr;
    map* info;
    directive_handler start_directive_handler;
    directive_handler stop_directive_handler;
    directive_handler restart_directive_handler;
} client_config;

typedef struct netrix_client {
    client_config config;
    deque* message_queue;
    http_server* http_server;
} netrix_client;

netrix_client* create_client(client_config);

int send_message(netrix_client*, netrix_message*);
int sent_event(netrix_client*, netrix_event*);
bool have_message(netrix_client*);
netrix_message* receive_message(netrix_client*);
void free_client(netrix_client*);

#endif