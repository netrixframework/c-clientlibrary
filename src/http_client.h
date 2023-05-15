#ifndef HTTP_CLIENT_H_
#define HTTP_CLIENT_H_

#include "ds.h"

typedef struct http_request {
    char* body;
    char* method;
    map* headers;
    char* url;
} http_request;

typedef struct http_response {
    long error_code;
    char* error;
    char* response_body;
} http_response;

http_response* http_create_response(void);
void http_free_response(http_response*);

http_response* http_post(char* url, char* body, map* headers);
http_response* http_get(char* url, map* headers);

http_response* http_do(http_request*);

#endif