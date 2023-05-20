#ifndef HTTP_CLIENT_H_
#define HTTP_CLIENT_H_

#include "ds.h"

typedef struct netrix_http_request {
    char* body;
    char* method;
    netrix_map* headers;
    char* url;
} netrix_http_request;

typedef struct netrix_http_response {
    long error_code;
    char* error;
    char* response_body;
} netrix_http_response;

netrix_http_response* netrix_http_create_response(void);
void netrix_http_free_response(netrix_http_response*);

netrix_http_response* netrix_http_post(char* url, char* body, netrix_map* headers);
netrix_http_response* netrix_http_get(char* url, netrix_map* headers);

netrix_http_response* netrix_http_do(netrix_http_request*);

#endif