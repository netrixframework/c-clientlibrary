
#ifndef NETRIX_CLIENT_H_
#define NETRIX_CLIENT_H_

#include "netrix_types.h"
#include "ds.h"
#include "http_server.h"
#include <stdbool.h>
#include <pthread.h>

typedef enum {NETRIX_START_DIRECTIVE, NETRIX_STOP_DIRECTIVE, NETRIX_RESTART_DIRECTIVE} NETRIX_DIRECTIVE;

typedef void (*directive_handler)(NETRIX_DIRECTIVE, void*);

typedef struct netrix_client_config {
    const char* id;
    const char* netrix_addr;
    const char* listen_addr;
    netrix_map* info;
    void* user_data;
    directive_handler directive_handler;
} netrix_client_config;

typedef struct netrix_client {
    netrix_client_config config;
    netrix_deque* message_queue;
    netrix_http_server* http_server;
    netrix_map* message_counter;
    pthread_t server_thread;
} netrix_client;

netrix_client* netrix_create_client(netrix_client_config);
int netrix_run_client(netrix_client*);
void netrix_signal_client(netrix_client*, int);
long netrix_send_message(netrix_client*, netrix_message*);
long netrix_send_event(netrix_client*, netrix_event*);
bool netrix_have_message(netrix_client*);
netrix_message* netrix_receive_message(netrix_client*);
void netrix_free_client(netrix_client*);

#endif