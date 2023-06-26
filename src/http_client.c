#include "http_client.h"
#include <curl/curl.h>
#include <stdio.h>
#include <string.h>

netrix_http_response* netrix_http_create_response(void) {
    netrix_http_response* resp = malloc(sizeof(netrix_http_response));
    return resp;
}

void netrix_http_free_response(netrix_http_response* resp) {
    free(resp);
}

typedef struct http_post_fn_data {
    bool done;
    netrix_http_response* response;
} http_post_fn_data;

netrix_http_response* netrix_http_post(char* url, char* body, netrix_map* headers) {
    netrix_http_request request;
    request.method = "POST";
    request.body = body;
    request.headers = headers;
    request.url = url;

    return netrix_http_do(&request);
}

netrix_http_response* netrix_http_get(char* url, netrix_map* headers) {
    netrix_http_request request;
    request.method = "GET";
    request.url = url;
    request.headers = headers;

    return netrix_http_do(&request);
}

static size_t request_cb(char* ptr, size_t size, size_t nemb, netrix_string* resp) {
    // Send the request and update the response
    size_t real_size = size*nemb;
    netrix_string_appendn(resp, ptr, real_size);
    return real_size;
}

netrix_http_response* netrix_http_do(netrix_http_request* req) {
    if (req == NULL) {
        netrix_http_response* resp = netrix_http_create_response();
        resp->error = "Empty request";
        resp->error_code = 1;
        resp->response_body = NULL;
        return resp;
    }

    netrix_http_response* resp = netrix_http_create_response();
    curl_global_init(CURL_GLOBAL_ALL);

    CURL* handle = curl_easy_init();
    if (handle) {
        curl_easy_setopt(handle, CURLOPT_URL, req->url);
        netrix_string* resp_s = netrix_create_string(NULL);
        
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, resp_s);
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, request_cb);

        struct curl_slist* headers = NULL;
        if(netrix_map_size(req->headers) != 0) {
            netrix_deque_elem* elem = netrix_map_iterator(req->headers);
            for (;elem != NULL; elem = elem->next) {
                netrix_map_elem* header = (netrix_map_elem*) elem->elem;
                netrix_string* header_s = netrix_create_string((char*) header->key);
                netrix_string_append(header_s, ": ");
                netrix_string_append(header_s, (char*) header->value);
                headers = curl_slist_append(headers, header_s->ptr);
            }

            curl_easy_setopt(handle, CURLOPT_HEADER, headers);
        }
        
        if(strcmp(req->method, "POST") == 0) {
            curl_easy_setopt(handle, CURLOPT_POSTFIELDS, req->body);
        }
        CURLcode res = curl_easy_perform(handle);
        if (res == CURLE_OK) {
            long resp_code;
            curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &resp_code);
            if (resp_code >= 400) {
                resp->error_code = resp_code;
                resp->error = netrix_string_str(resp_s);
            } else {
                resp->error_code = 0;
                resp->error = NULL;
                resp->response_body = netrix_string_str(resp_s);
            }
        } else {
            resp->error_code = res;
            resp->error = curl_easy_strerror(res);
        }
        netrix_free_string(resp_s);
        curl_easy_cleanup(handle);
    }
    curl_global_cleanup();
    return resp;
}