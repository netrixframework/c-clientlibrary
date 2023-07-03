#include "http_server.h"
#include "ds.h"
#include "mongoose.h"
#include <pthread.h>

typedef struct handler_thread_data {
    netrix_deque* queue;
    char* body;
    void* fn_data;
    netrix_http_handler handler;
} handler_thread_data;

netrix_http_reply* netrix_http_create_reply(void) {
    netrix_http_reply* reply = malloc(sizeof(netrix_http_reply));
    reply->headers = NULL;
    reply->status_code = 200;
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
    server->signal = 0;

    mg_mgr_init(server->mg_mgr);
    mg_log_set(MG_LL_INFO);
    mg_log_set_fn(mg_log_fn, NULL);

    return server;
}

netrix_http_reply* default_handler(char* message, void* fn_data) {
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

void* handler_thread(void* data) {
    handler_thread_data* handler_data = (handler_thread_data *) data;

    netrix_http_handler handler = handler_data->handler;
    netrix_http_reply* reply = handler(handler_data->body, handler_data->fn_data);

    netrix_cdeque_push_back(handler_data->queue, (void *) reply);

    while(netrix_cdeque_size(handler_data->queue) != 0) usleep(100);

    netrix_free_deque(handler_data->queue);
    free(handler_data);

    return NULL;
}

void start_handler_thread(handler_thread_data* data) {
    pthread_t thread_id = (pthread_t) 0;
    pthread_attr_t attr;
    (void) pthread_attr_init(&attr);
    (void) pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&thread_id, &attr, handler_thread, data);
    pthread_attr_destroy(&attr);
}

void connection_handler(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message* http_msg = (struct mg_http_message*) ev_data;
        netrix_http_server* server = (netrix_http_server*) fn_data;
        const char* uri = strndup(http_msg->uri.ptr, http_msg->uri.len);
        netrix_http_handler handler = http_get_handler(server, uri);

        handler_thread_data *handler_data = malloc(sizeof(handler_thread_data));
        handler_data->fn_data = server->fn_data;
        handler_data->handler = handler;
        handler_data->queue = netrix_create_deque();
        char* body = strndup(http_msg->body.ptr, http_msg->body.len);
        handler_data->body = body;

        start_handler_thread(handler_data);
        *(void **) c->data = handler_data;
    } else if (ev == MG_EV_POLL) {
        handler_thread_data *handler_data = *(handler_thread_data **) c->data;

        if (handler_data != NULL && netrix_cdeque_size(handler_data->queue) != 0) {
            netrix_http_reply* reply = netrix_cdeque_pop_front(handler_data->queue);
            mg_http_reply(c, reply->status_code, reply->headers, reply->body);
            netrix_http_free_reply(reply);

            *(void **) c->data = NULL;
        }
    }
}

void netrix_http_add_handler(netrix_http_server* s, const char* uri, netrix_http_handler handler) {
    netrix_map_add(s->handlers, uri, handler);
}

void* netrix_http_listen(netrix_http_server* s) {
    mg_http_listen(s->mg_mgr, s->listen_addr, connection_handler,s);
    while (s->signal == 0) mg_mgr_poll(s->mg_mgr, 10);
    return NULL;
}

void netrix_http_signal(netrix_http_server* server, int signal) {
    server->signal = signal;
}

void netrix_http_free_server(netrix_http_server* s) {
    mg_mgr_free(s->mg_mgr);
    free(s->mg_mgr);
    free(s);
}