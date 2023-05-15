#include "netrix_types.h"
#include <stddef.h>
#include <string.h>
#include <json-c/json.h>

netrix_message* create_message(char* to, char* data, char* type) {
    netrix_message* new_message = malloc(sizeof(netrix_message));
    new_message->to = to;
    new_message->data = data;
    new_message->type = type;
    return new_message;
}

char* serialize_message(netrix_message* message) {
    json_object* obj = json_object_new_object();

    json_object_object_add(obj, "from", json_object_new_string(message->from));
    json_object_object_add(obj, "to", json_object_new_string(message->to));
    json_object_object_add(obj, "data", json_object_new_string(message->data));
    json_object_object_add(obj, "type", json_object_new_string(message->type));
    json_object_object_add(obj, "id", json_object_new_string(message->id));

    char* serialized = json_object_to_json_string(obj);
    char* result = malloc(strlen(serialized));
    strncpy(result, serialized, strlen(serialized));

    json_object_put(obj);

    return result;
}

netrix_message* deserialize_message(char* message_s) {
    // TODO: Need to do error handling when parsing
    json_object* obj = json_tokener_parse(message_s);
    if (obj == NULL) {
        return NULL;
    }
    char* from, to, data, type, id;
    json_object* j_from = json_object_object_get(obj, "from");
    json_object* j_to = json_object_object_get(obj, "to");
    json_object* j_data = json_object_object_get(obj, "data");
    json_object* j_type = json_object_object_get(obj, "type");
    json_object* j_id = json_object_object_get(obj, "id");

    from = strdup(json_object_get_string(j_from));
    to = strdup(json_object_get_string(j_to));
    data = strdup(json_object_get_string(j_data));
    type = strdup(json_object_get_string(j_type));
    id = strdup(json_object_get_string(j_id));

    json_object_put(obj);

    netrix_message* message = create_message(to, data, type);
    message->from = from;
    message->id = id;

    return message;
}

void free_message(netrix_message* message) {
    free(message);
}

netrix_event* create_event(char* type, map* params, long timestamp) {
    netrix_event* new_event = malloc(sizeof(netrix_event));
    new_event->type = type;
    new_event->params = params;
    new_event->timestamp = timestamp;
    return new_event;
}

char* serialize_event(netrix_event* event) {
    json_object* obj = json_object_new_object();

    json_object_object_add(obj, "replica", json_object_new_string(event->replica));
    json_object_object_add(obj, "type", json_object_new_string(event->type));
    json_object_object_add(obj, "timestamp", json_object_new_double((double) event->timestamp));

    struct json_object* params_obj = json_object_new_object();
    if(map_size(event->params) != 0) {
        deque_elem* e = map_iterator(event->params);
        for(;e!= NULL; e = e->next) {
            map_elem* map_e = (map_elem*) e->elem;
            json_object_object_add(params_obj, map_e->key, json_object_new_string((char*) map_e->value));
        }
    }
    json_object_object_add(obj, "params", params_obj);

    char* serialized = json_object_to_json_string(obj);
    char* result = malloc(strlen(serialized));
    strncpy(result, serialized, strlen(serialized));

    json_object_put(obj);

    return result;
}

void free_event(netrix_event* event) {
    free(event);
}