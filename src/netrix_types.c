#include "netrix_types.h"
#include <stddef.h>

netrix_message* create_message(char* to, char* data, char* type) {
    netrix_message* new_message = malloc(sizeof(netrix_message));
    new_message->to = to;
    new_message->data = data;
    new_message->type = type;
    return new_message;
}

char* serialize_message(netrix_message* message) {
    // TODO: json serialize message
    return "";
}

netrix_message* deserialize_message(char* message_s) {
    // TODO: json deserialize message
    return NULL;
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

void free_event(netrix_event* event) {
    free(event);
}