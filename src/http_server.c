#include "http_server.h"
#include "ds.h"
#include "mongoose.h"

netrix_http_reply* netrix_http_create_reply(void) {
    netrix_http_reply* reply = malloc(sizeof(netrix_http_reply));
    return reply;
}

void netrix_http_free_reply(netrix_http_reply* r) {
    free(r);
}

void mg_log_fn(char ch, void* param) {
    return;
}

netrix_http_server* netrix_http_create_server(const char* listen_addr, void* fn_data) {
    netrix_http_server* server = malloc(sizeof(netrix_http_server));
    server->listen_addr = listen_addr;
    server->handlers = netrix_create_map();
    server->mg_mgr = malloc(sizeof(mg_mgr));
    server->fn_data = fn_data;

    mg_mgr_init(server->mg_mgr);
    mg_log_set(MG_LL_INFO);
    mg_log_set_fn(mg_log_fn, NULL);

    return server;
}

netrix_http_reply* default_handler(struct mg_http_message* message, void* fn_data) {
    netrix_http_reply* reply = netrix_http_create_reply();
    reply->body = "{\"msg\":\"Not found\"}";
    reply->status_code = 404;
    reply->headers = "Content-Type: application/json\r\n";
    return reply;
}

netrix_http_handler http_get_handler(netrix_http_server* server, const char* uri) {
    if (netrix_map_size(server->handlers) == 0) {
        return default_handler;
    }
    void* handler = netrix_map_get(server->handlers, uri);
    if (handler == NULL) {
        return default_handler;
    }
    return (netrix_http_handler) handler;
}

void connection_handler(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message* http_msg = (struct mg_http_message*) ev_data;
        netrix_http_server* server = (netrix_http_server*) fn_data;
        netrix_http_handler handler = http_get_handler(server, http_msg->uri.ptr);
        
        netrix_http_reply* reply = handler(http_msg, server->fn_data);
        mg_http_reply(c, reply->status_code, reply->headers, reply->body);

        netrix_http_free_reply(reply);
    }
    (void) fn_data;
}

void netrix_http_add_handler(netrix_http_server* s, const char* uri, netrix_http_handler handler) {
    netrix_map_add(s->handlers, uri, handler);
}

void netrix_http_listen(netrix_http_server* s) {
    mg_http_listen(s->mg_mgr, s->listen_addr, connection_handler,s);
    return;
}

void netrix_http_free_server(netrix_http_server* s) {
    mg_mgr_free(s->mg_mgr);
    free(s);
}