#include "client.h"
#include "http_client.h"
#include "http_server.h"
#include <string.h>
#include <stdio.h>
#include <json-c/json.h>

char *netrix_directive_name(NETRIX_DIRECTIVE d) {
    switch (d)  
    {
    case NETRIX_START_DIRECTIVE:
        return "Start";
    case NETRIX_STOP_DIRECTIVE:
        return "Stop";
    case NETRIX_RESTART_DIRECTIVE:
        return "Restart";
    default:
        break;
    }
    return "";
}

netrix_http_reply* handle_message(char *body, void* fn_data) {
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

netrix_http_reply* handle_directive(char *body, void* fn_data) {
    printf("Received a directive from netrix\n");

    json_object* obj = json_tokener_parse(body);
    json_object* action_obj = json_object_object_get(obj, "action");
    const char* action = strdup(json_object_get_string(action_obj));

    printf("Directive: %s\n", action);

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

void* run_server(void* arg) {
    netrix_http_server* http_server = (netrix_http_server *) arg;
    return netrix_http_listen(http_server);
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
    new_client->server_thread = (pthread_t) 0;

    if(netrix_send_replica_info(new_client, 0) != 0) {
        return NULL;
    }
    pthread_create(&new_client->server_thread, NULL, run_server, server);

    return new_client;
}

int netrix_run_client(netrix_client* c) {
    return netrix_send_replica_info(c, 1);
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
        int *count_ptr = malloc(sizeof(int));
        *count_ptr = 0;
        netrix_map_add(c->message_counter, netrix_string_str(key), count_ptr);
    }

    int *count_ptr = (int *) netrix_map_get(c->message_counter, netrix_string_str(key));
    int count = *count_ptr;
    *count_ptr = *count_ptr + 1;
    netrix_map_add(c->message_counter, netrix_string_str(key), count_ptr);

    char count_s[12];
    sprintf(count_s, "%d", count);

    netrix_string* val = netrix_string_append(key, "_");
    netrix_string_append(val, count_s);

    char* id = netrix_string_str(val);
    netrix_free_string(val);
    return id;
}

long netrix_send_message(netrix_client* c, netrix_message* message) {
    message->from = strdup(c->config.id);
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

    char *serialized_event = netrix_serialize_event(event);
    netrix_http_response* response = netrix_http_post(netrix_string_str(addr), serialized_event, headers);

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