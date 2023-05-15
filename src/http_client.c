#include "http_client.h"
#include "mongoose.h"

typedef struct http_post_fn_data {
    bool done;
    http_response* response;
} http_post_fn_data;

http_response* http_post(http_post_request* request) {
    // TODO: send http post request with http_post_fn_data as fn_data
    return NULL;
}

void post_caller(struct mg_connection* c, int ev, void* ev_data, void* fn_data) {
    // Send the request and update the response
}