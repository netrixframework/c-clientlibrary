#include "client.h"
#include "http_client.h"
#include "http_server.h"
#include <string.h>


netrix_http_reply* handle_message(struct mg_http_message* http_message, void* fn_data) {
    // TODO: add message to the message queue
    return NULL;
}

netrix_http_reply* handle_directive(struct mg_http_message* http_message, void* fn_data) {
    // TODO: check if handlers are not null and invoke the right handler
    return NULL;
}

netrix_client* create_client(client_config config) {
    netrix_client* new_client = malloc(sizeof(netrix_client));
    new_client->config = config;
    new_client->message_queue = netrix_create_deque();
    netrix_http_server* server = netrix_http_create_server(config.listen_addr, new_client);
    netrix_http_add_handler(server, "/message", handle_message);
    netrix_http_add_handler(server, "/directive", handle_directive);
    new_client->http_server = server;
    new_client->message_counter = netrix_create_map();

    return new_client;
}

long netrix_run_client(netrix_client* c) {
    // TODO: initiate a request to replica endpoint indicating ready after starting the http server on a separate thread
    return 0;
}

char* get_message_id(netrix_client* c, char* from, char* to) {
    netrix_string* key = netrix_create_string(from);
    netrix_string_append(key, "_");
    netrix_string_append(key, to);

    if(!netrix_map_exists(c->message_counter, netrix_string_str(key))) {
        netrix_map_add(c->message_counter, netrix_string_str(key), 0);
    }

    int count = (int) netrix_map_get(c->message_counter, netrix_string_str(key));
    netrix_map_add(c->message_counter, netrix_string_str(key), (void*) count+1);

    char count_s[3];
    sprintf(count_s, "%d", count);

    netrix_string* val = netrix_string_append(key, "_");
    netrix_string_append(val, count_s);

    char* id = netrix_string_str(val);
    netrix_free_string(val);
    return id;
}

long netrix_send_message(netrix_client* c, netrix_message* message) {
    message->from = c->config.id;
    message->id = get_message_id(c, message->from, message->to);

    netrix_string* addr = netrix_create_string(NULL);
    netrix_string_append(addr, "http://");
    netrix_string_append(addr, c->config.netrix_addr);
    netrix_string_append(addr, "/message");

    netirx_map* headers = netrix_create_map();
    netrix_map_add(headers, "Content-Type", "application/json");

    netrix_http_response* response = netrix_http_post(netrix_string_str(addr), netrix_serialize_message(message), headers);

    netrix_free_string(addr);

    return response->error_code;
}

long netrix_sent_event(netrix_client* c, netrix_event* event) {
    event->replica = c->config.id;

    netrix_string* addr = netrix_create_string(NULL);
    netrix_string_append(addr, "http://");
    netrix_string_append(addr, c->config.netrix_addr);
    netrix_string_append(addr, "/event");

    netirx_map* headers = netrix_create_map();
    netrix_map_add(headers, "Content-Type", "application/json");

    netrix_http_response* response = netrix_http_post(netrix_string_str(addr), netrix_serialize_event(event), headers);

    netrix_free_string(addr);

    return response->error_code;
}

bool netrix_have_message(netrix_client* c) {
    return c->message_queue->size == 0;
}

netrix_message* netrix_receive_message(netrix_client* c) {
    void* val = netrix_deque_pop_front(c->message_queue);
    if (val != NULL) {
        return (netrix_message*) val;
    }
    return NULL;
}

void netrix_free_client(netrix_client* c) {
    netrix_free_deque(c->message_queue);
    netrix_free_map(c->message_counter);
    netrix_http_free_server(c->http_server);
    free(c);
}