#include "client.h"
#include "http_client.h"
#include "http_server.h"
#include <string.h>
#include <stdio.h>
#include <json-c/json.h>

netrix_http_reply* handle_message(struct mg_http_message* http_message, void* fn_data) {
    char* body = strndup(http_message->body.ptr, http_message->body.len);
    netrix_message* message = netrix_deserialize_message(body);
    netrix_client* client = (netrix_client*) fn_data;
    netrix_cdeque_push_back(client->message_queue, message);

    netrix_map* params = netrix_create_map();
    netrix_map_add(params, "message_id", strdup(message->id));
    netrix_event* e = netrix_create_event("MessageReceive", params);
    netrix_send_event(client, e);
    netrix_free_event(e);
    netrix_free_map(params);

    netrix_http_reply* reply = netrix_http_create_reply();
    reply->body = "OK";
    return reply;
}

netrix_http_reply* handle_directive(struct mg_http_message* http_message, void* fn_data) {
    char* body = strndup(http_message->body.ptr, http_message->body.len);
    json_object* obj = json_tokener_parse(body);
    json_object* action_obj = json_object_object_get(obj, "action");
    const char* action = strdup(json_object_get_string(action_obj));

    netrix_client* client = (netrix_client *) fn_data;
    NETRIX_DIRECTIVE directive;
    if (strcmp(action, "START") == 0) {
        directive = NETRIX_START_DIRECTIVE;
    } else if (strcmp(action, "STOP") == 0) {
        directive = NETRIX_STOP_DIRECTIVE;
    } else if(strcmp(action, "RESTART") == 0) {
        directive = NETRIX_RESTART_DIRECTIVE;
    }

    if (client->config.directive_handler != NULL) {
        client->config.directive_handler(directive, client->config.user_data);
    }

    netrix_http_reply* reply = netrix_http_create_reply();
    reply->body = "OK";
    return reply;
}

int netrix_send_replica_info(netrix_client* client, int ready) {
    json_object* obj = json_object_new_object();

    json_object_object_add(obj, "id", json_object_new_string(client->config.id));
    json_object_object_add(obj, "ready", json_object_new_boolean(ready));
    json_object_object_add(obj, "addr", json_object_new_string(client->config.listen_addr));
    json_object* info_obj = json_object_new_object();
    if(netrix_map_size(client->config.info)!=0) {
        for(netrix_deque_elem* e= netrix_map_iterator(client->config.info); e != NULL; e = e->next) {
            netrix_map_elem* map_e = (netrix_map_elem*) e->elem;
            json_object_object_add(info_obj, strdup(map_e->key), json_object_new_string(strdup((char*) map_e->value)));
        }
    }
    json_object_object_add(obj, "info", info_obj);
    
    char* replica_data = strdup(json_object_to_json_string(obj));
    json_object_put(obj);
    

    netrix_string* addr = netrix_create_string(NULL);
    netrix_string_append(addr, "http://");
    netrix_string_append(addr, client->config.netrix_addr);
    netrix_string_append(addr, "/replica");

    netrix_map* headers = netrix_create_map();
    netrix_map_add(headers, "Content-Type", "application/json");

    netrix_http_response* response = netrix_http_post(netrix_string_str(addr), replica_data, headers);

    netrix_free_string(addr);
    netrix_free_map(headers);

    return response->error_code;
}

netrix_client* netrix_create_client(netrix_client_config config) {
    netrix_client* new_client = malloc(sizeof(netrix_client));
    new_client->config = config;
    new_client->message_queue = netrix_create_deque();
    netrix_http_server* server = netrix_http_create_server(config.listen_addr, new_client);
    netrix_http_add_handler(server, "/message", handle_message);
    netrix_http_add_handler(server, "/directive", handle_directive);
    new_client->http_server = server;
    new_client->message_counter = netrix_create_map();

    if(netrix_send_replica_info(new_client, 0) != 0) {
        return NULL;
    }

    return new_client;
}

void* run_server(void* arg) {
    netrix_http_server* http_server = (netrix_http_server *) arg;
    return netrix_http_listen(http_server);
}

int netrix_run_client(netrix_client* c) {
    if(netrix_send_replica_info(c, 1) != 0) {
        return -1;
    }
    return pthread_create(&c->server_thread, NULL, run_server, c->http_server);
}

void netrix_signal_client(netrix_client* c, int signal) {
    if (signal > 0) {
        netrix_http_signal(c->http_server, signal);
        pthread_join(c->server_thread, NULL);
    }
}

char* get_message_id(netrix_client* c, char* from, char* to) {
    // TODO: move this to thread safe code.
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
    strcpy(message->from, c->config.id);
    message->id = get_message_id(c, message->from, message->to);

    netrix_map* params = netrix_create_map();
    netrix_map_add(params, "message_id", strdup(message->id));
    netrix_event* e = netrix_create_event("MessageSend", params);
    netrix_send_event(c, e);
    netrix_free_event(e);
    netrix_free_map(params);

    netrix_string* addr = netrix_create_string(NULL);
    netrix_string_append(addr, "http://");
    netrix_string_append(addr, c->config.netrix_addr);
    netrix_string_append(addr, "/message");

    netrix_map* headers = netrix_create_map();
    netrix_map_add(headers, "Content-Type", "application/json");

    netrix_http_response* response = netrix_http_post(netrix_string_str(addr), netrix_serialize_message(message), headers);

    netrix_free_string(addr);
    netrix_free_map(headers);

    return response->error_code;
}

long netrix_send_event(netrix_client* c, netrix_event* event) {
    event->replica = c->config.id;

    netrix_string* addr = netrix_create_string(NULL);
    netrix_string_append(addr, "http://");
    netrix_string_append(addr, c->config.netrix_addr);
    netrix_string_append(addr, "/event");

    netrix_map* headers = netrix_create_map();
    netrix_map_add(headers, "Content-Type", "application/json");

    netrix_http_response* response = netrix_http_post(netrix_string_str(addr), netrix_serialize_event(event), headers);

    netrix_free_string(addr);
    netrix_free_map(headers);

    return response->error_code;
}

bool netrix_have_message(netrix_client* c) {
    return netrix_cdeque_size(c->message_queue) == 0;
}

netrix_message* netrix_receive_message(netrix_client* c) {
    void* val = netrix_cdeque_pop_front(c->message_queue);
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