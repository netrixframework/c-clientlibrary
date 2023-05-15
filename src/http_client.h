#ifndef HTTP_CLIENT_H_
#define HTTP_CLIENT_H_

#include "mongoose.h"

typedef struct http_post_request {
    char* body;
    char* headers;
    char* url;
} http_post_request;

typedef struct http_post_response {
    int error_code;
    char* error;
    struct mg_http_message* resonse_msg;
} http_response;

http_response* http_post(http_post_request*);

#endif