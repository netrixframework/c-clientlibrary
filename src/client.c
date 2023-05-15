#include "client.h"
#include "http_client.h"
#include "http_server.h"
#include <string.h>

netrix_client* create_client(client_config config) {
    netrix_client* new_client = malloc(sizeof(netrix_client));
    new_client->config = config;
    new_client->message_queue = create_deque();
    http_server* server = http_create_server(config.listen_addr, new_client);
    http_add_handler(server, "/message", handle_message);
    http_add_handler(server, "/directive", handle_directive);
    new_client->http_server = server;

    return new_client;
}

int send_message(netrix_client* c, netrix_message* message) {
    // TODO Need to add appropriate headers
    char* addr = "http://";
    strcat(addr, c->config.netrix_addr);
    strcat(addr, "/message");
    http_post_request request;
    request.url = addr;
    request.body = serialize_message(message);
    http_response* response = http_post(&request);
    return response->error_code;
}


int sent_event(netrix_client* c, netrix_event* event) {
    // TODO Need to add appropriate headers
    char* addr = "http://";
    strcat(addr, c->config.netrix_addr);
    strcat(addr, "/event");
    http_post_request request;
    request.url = addr;
    request.body = serialize_event(event);
    http_response* response = http_post(&request);
    return response->error_code;
}

bool have_message(netrix_client* c) {
    return c->message_queue->size == 0;
}

netrix_message* receive_message(netrix_client* c) {
    void* val = deque_pop_front(c->message_queue);
    if (val != NULL) {
        return (netrix_message*) val;
    }
    return NULL;
}

void free_client(netrix_client* c) {
    free_deque(c->message_queue);
    http_free_server(c->http_server);
    free(c);
}

http_reply* handle_message(struct mg_http_message* http_message, void* fn_data) {
    // TODO: add message to the message queue
    return NULL;
}

http_reply* handle_directive(struct mg_http_message* http_message, void* fn_data) {
    // TODO: check if handlers are not null and invoke the right handler
    return NULL;
}