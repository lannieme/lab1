//
// Created by Huai-Che Lu on 2/28/17.
//

#ifndef CS5450_LAB1_RESPONSE_H
#define CS5450_LAB1_RESPONSE_H

#include <stdbool.h>
#include <sys/types.h>

#define MAX_MESSAGE_BODY_SIZE                       10000

#define HTTP_STATUS_CODE_OK                         200
#define HTTP_STATUS_CODE_NOT_FOUND                  404
#define HTTP_STATUS_CODE_LENGTH_REQUIRED            411
#define HTTP_STATUS_CODE_INTERNAL_SERVER_ERROR      500
#define HTTP_STATUS_CODE_NOT_IMPLEMENTED            501
#define HTTP_STATUS_CODE_SERVICE_UNAVAILABLE        503
#define HTTP_STATUS_CODE_HTTP_VERSION_NOT_SUPPORTED 505
#define HTTP_STATUS_CODE_UNAUTHORIZED               401
// HTTP Response Header Field
typedef struct ResponseHeader {
    char key[4096];
    char value[4096];
} ResponseHeader;

// HTTP Response Header
typedef struct Response {

    char http_version[50];
    int http_status_code;
    char http_status_description[50];

    int n_headers; // number
    int c_headers; // capacity
    ResponseHeader *headers;

    size_t message_body_size;
    char message_body[MAX_MESSAGE_BODY_SIZE];
} Response;

Response *create_response(char *http_version, int http_status_code);
void add_response_header(Response *response, char *key, char *value);
void add_response_header_i(Response *response, char *key, int value);
void get_response_str(Response *response, char *out_str, bool header_only);
void set_response_status_code(Response *response, int status_code);
void set_response_message_body(Response *response, char *message_body, size_t size);
void set_response_message_body_size(Response *response, size_t size);
void destroy_response(Response *response);

#endif //CS5450_LAB1_RESPONSE_H
