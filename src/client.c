#include "client.h"
#include "http_client.h"
#include "http_server.h"
#include <string.h>


http_reply* handle_message(struct mg_http_message* http_message, void* fn_data) {
    // TODO: add message to the message queue
    return NULL;
}

http_reply* handle_directive(struct mg_http_message* http_message, void* fn_data) {
    // TODO: check if handlers are not null and invoke the right handler
    return NULL;
}

netrix_client* create_client(client_config config) {
    netrix_client* new_client = malloc(sizeof(netrix_client));
    new_client->config = config;
    new_client->message_queue = create_deque();
    http_server* server = http_create_server(config.listen_addr, new_client);
    http_add_handler(server, "/message", handle_message);
    http_add_handler(server, "/directive", handle_directive);
    new_client->http_server = server;
    new_client->message_counter = create_map();

    return new_client;
}

long run_client(netrix_client* c) {
    // TODO: initiate a request to replica endpoint indicating ready after starting the http server on a separate thread
    return 0;
}

char* get_message_id(netrix_client* c, char* from, char* to) {
    string* key = create_string(from);
    string_append(key, "_");
    string_append(key, to);

    if(!map_exists(c->message_counter, string_str(key))) {
        map_add(c->message_counter, string_str(key), 0);
    }

    int count = (int) map_get(c->message_counter, string_str(key));
    map_add(c->message_counter, string_str(key), (void*) count+1);

    char count_s[3];
    sprintf(count_s, "%d", count);

    string* val = string_append(key, "_");
    string_append(val, count_s);

    char* id = string_str(val);
    free_string(val);
    return id;
}

long send_message(netrix_client* c, netrix_message* message) {
    message->from = c->config.id;
    message->id = get_message_id(c, message->from, message->to);

    string* addr = create_string(NULL);
    string_append(addr, "http://");
    string_append(addr, c->config.netrix_addr);
    string_append(addr, "/message");

    map* headers = create_map();
    map_add(headers, "Content-Type", "application/json");

    http_response* response = http_post(string_str(addr), serialize_message(message), headers);

    free_string(addr);

    return response->error_code;
}

long sent_event(netrix_client* c, netrix_event* event) {
    event->replica = c->config.id;

    string* addr = create_string(NULL);
    string_append(addr, "http://");
    string_append(addr, c->config.netrix_addr);
    string_append(addr, "/event");

    map* headers = create_map();
    map_add(headers, "Content-Type", "application/json");

    http_response* response = http_post(string_str(addr), serialize_event(event), headers);

    free_string(addr);

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
    free_map(c->message_counter);
    http_free_server(c->http_server);
    free(c);
}