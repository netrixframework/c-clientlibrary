#include "http_server.h"
#include "ds.h"
#include "mongoose.h"

http_server* http_create_server(char* listen_addr, void* fn_data) {
    http_server* server = malloc(sizeof(http_server));
    server->listen_addr = listen_addr;
    server->handlers = create_map();
    server->mg_mgr = malloc(sizeof(mg_mgr));
    server->fn_data = fn_data;

    mg_mgr_init(server->mg_mgr);
    mg_log_set(MG_LL_INFO);
    mg_log_set_fn(mg_log_fn, NULL);

    return server;
}

http_handler http_get_handler(http_server* server, const char* uri) {
    if (map_size(server->handlers) == 0) {
        return default_handler;
    }
    void* handler = map_get(server->handlers, uri);
    if (handler == NULL) {
        return default_handler;
    }
    return (http_handler) handler;
}

void connection_handler(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message* http_msg = (struct mg_http_message*) ev_data;
        http_server* server = (http_server*) fn_data;
        http_handler handler = http_get_handler(server, http_msg->uri.ptr);
        
        http_reply* reply = handler(http_msg, server->fn_data);
        mg_http_reply(c, reply->status_code, reply->headers, reply->body);

        free(reply);
    }
    (void) fn_data;
}

http_reply* *default_handler(struct mg_http_message* message, void* fn_data) {
    http_reply reply;
    reply.body = "{\"msg\":\"Not found\"}";
    reply.status_code = 404;
    reply.headers = "Content-Type: application/json\r\n";
    return &reply;
}

void http_add_handler(http_server* s, const char* uri, http_handler handler) {
    map_add(s->handlers, uri, handler);
}

void mg_log_fn(char ch, void* param) {
    return;
}

void http_listen(http_server* s) {
    mg_http_listen(s->mg_mgr, s->listen_addr, connection_handler,s);
    return;
}

void http_free_server(http_server* s) {
    mg_mgr_free(s->mg_mgr);
    free(s);
}