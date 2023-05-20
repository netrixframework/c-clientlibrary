
#ifndef NETRIX_CLIENT_H_
#define NETRIX_CLIENT_H_

#include "netrix_types.h"
#include "ds.h"
#include "http_server.h"
#include <stdbool.h>
#include <pthread.h>

typedef void (*directive_handler)(char*);

typedef struct client_config {
    const char* id;
    const char* netrix_addr;
    const char* listen_addr;
    netrix_map* info;
    directive_handler start_directive_handler;
    directive_handler stop_directive_handler;
    directive_handler restart_directive_handler;
} client_config;

typedef struct netrix_client {
    client_config config;
    netrix_deque* message_queue;
    netrix_http_server* http_server;
    netrix_map* message_counter;
    pthread_t server_thread;
} netrix_client;

netrix_client* create_client(client_config);
int netrix_run_client(netrix_client*);
void netrix_signal_client(netrix_client*, int);
long netrix_send_message(netrix_client*, netrix_message*);
long netrix_send_event(netrix_client*, netrix_event*);
bool netrix_have_message(netrix_client*);
netrix_message* netrix_receive_message(netrix_client*);
void netrix_free_client(netrix_client*);

#endif