#ifndef NETRIX_HTTP_SERVER_H_
#define NETRIX_HTTP_SERVER_H_

#include "ds.h"
#include "mongoose.h"

typedef struct mg_mgr mg_mgr;

typedef struct http_reply {
    int status_code;
    char* headers;
    char* body;
} http_reply;

http_reply* http_create_reply(void);
void http_free_reply(http_reply*);

typedef http_reply* (*http_handler)(struct mg_http_message*, void* fn_data);

typedef struct http_server {
    const char* listen_addr;
    map* handlers;
    mg_mgr* mg_mgr;
    void* fn_data;
} http_server;

http_server* http_create_server(const char*, void* fn_data);
void http_add_handler(http_server*, const char*, http_handler);
void http_listen(http_server*);
void http_free_server(http_server*);

#endif