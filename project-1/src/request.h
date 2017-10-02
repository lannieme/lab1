//
// Created by Huai-Che Lu on 2/26/17.
//

#ifndef CS5450_LAB1_REQUEST_H
#define CS5450_LAB1_REQUEST_H

// HTTP Request Methods
typedef enum http_method_t {
    HEAD,
    GET,
    POST,
    UNIMPLEMENTED
} http_method_t;

// HTTP Request Header Field
typedef struct RequestHeader {
    char key[4096];
    char value[4096];
} RequestHeader;

// HTTP Request Header
typedef struct Request {
    http_method_t method_type;
    char http_method[50];
    char http_version[50];
    char http_uri[4096];

    int n_headers; // numbers
    int c_headers; // capacity
    RequestHeader *headers;
} Request;

void get_header_field_value(Request *request, char key[], char *out_str);
void destroy_request(Request *request);

#endif //CS5450_LAB1_REQUEST_H
