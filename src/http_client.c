#include "http_client.h"
#include <curl/curl.h>
#include <stdio.h>
#include <string.h>

http_response* http_create_response(void) {
    http_response* resp = malloc(sizeof(http_response));
    return resp;
}

void http_free_response(http_response* resp) {
    free(resp);
}

typedef struct http_post_fn_data {
    bool done;
    http_response* response;
} http_post_fn_data;

http_response* http_post(char* url, char* body, map* headers) {
    http_request request;
    request.method = "POST";
    request.body = body;
    request.headers = headers;
    request.url = url;

    return http_do(&request);
}

http_response* http_get(char* url, map* headers) {
    http_request request;
    request.method = "GET";
    request.url = url;
    request.headers = headers;

    return http_do(&request);
}

static size_t request_cb(char* ptr, size_t size, size_t nemb, string* resp) {
    // Send the request and update the response
    size_t real_size = size*nemb;
    string_appendn(resp, ptr, real_size);
    return real_size;
}

http_response* http_do(http_request* req) {
    if (req == NULL) {
        http_response* resp = http_create_response();
        resp->error = "Empty request";
        resp->error_code = 1;
        resp->response_body = NULL;
        return resp;
    }

    http_response* resp = http_create_response();
    curl_global_init(CURL_GLOBAL_ALL);

    CURL* handle = curl_easy_init();
    if (handle) {
        curl_easy_setopt(handle, CURLOPT_URL, req->url);
        string* resp_s = create_string(NULL);
        
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, resp_s);
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, request_cb);

        struct curl_slist* headers = NULL;
        if(map_size(req->headers) != 0) {
            deque_elem* elem = map_iterator(req->headers);
            for (;elem != NULL; elem = elem->next) {
                map_elem* header = (map_elem*) elem->elem;
                string* header_s = create_string((char*) header->key);
                string_append(header_s, ": ");
                string_append(header_s, (char*) header->value);
                headers = curl_slist_append(headers, header_s->ptr);
            }

            curl_easy_setopt(handle, CURLOPT_HEADER, headers);
        }
        
        if(strcmp(req->method, "POST") == 0) {
            curl_easy_setopt(handle, CURLOPT_POSTFIELDS, req->body);
        }
        CURLcode res = curl_easy_perform(handle);
        if (res != CURLE_OK) {
            long resp_code;
            curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &resp_code);
            if (resp_code >= 400) {
                resp->error_code = resp_code;
                resp->error = string_str(resp_s);
            } else {
                resp->error_code = 0;
                resp->error = NULL;
                resp->response_body = string_str(resp_s);
            }
        } else {
            resp->error_code = res;
            resp->error = curl_easy_strerror(res);
        }
        free_string(resp_s);
        curl_easy_cleanup(handle);
    }
    curl_global_cleanup();
    return resp;
}