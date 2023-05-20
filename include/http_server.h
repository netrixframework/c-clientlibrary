#ifndef NETRIX_HTTP_SERVER_H_
#define NETRIX_HTTP_SERVER_H_

#include "ds.h"
#include "mongoose.h"
#include <pthread.h>

typedef struct mg_mgr mg_mgr;

typedef struct netrix_http_reply {
    int status_code;
    char* headers;
    char* body;
} netrix_http_reply;

netrix_http_reply* netrix_http_create_reply(void);
void netrix_http_free_reply(netrix_http_reply*);

typedef netrix_http_reply* (*netrix_http_handler)(struct mg_http_message*, void* fn_data);

typedef struct netrix_http_server {
    const char* listen_addr;
    netrix_map* handlers;
    mg_mgr* mg_mgr;
    void* fn_data;
    int signal;
} netrix_http_server;

netrix_http_server* netrix_http_create_server(const char*, void* fn_data);
void netrix_http_add_handler(netrix_http_server*, const char*, netrix_http_handler);
void* netrix_http_listen(netrix_http_server*);
void netrix_http_signal(netrix_http_server*, int);
void netrix_http_free_server(netrix_http_server*);

#endif