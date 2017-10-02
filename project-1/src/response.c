//
// Created by Huai-Che Lu on 2/28/17.
//

#include "response.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void get_response_line_str(Response *response, char *out_str);
static void get_response_header_str(ResponseHeader *header, char *out_str);

static const char HTTP_STATUS_DESC_OK[]                         = "OK";
static const char HTTP_STATUS_DESC_NOT_FOUND[]                  = "Not Found";
static const char HTTP_STATUS_DESC_LENGTH_REQUIRED[]            = "Length Required";
static const char HTTP_STATUS_DESC_INTERNAL_SERVER_ERROR[]      = "Internal Server Error";
static const char HTTP_STATUS_DESC_NOT_IMPLEMENTED[]            = "Not Implemented";
static const char HTTP_STATUS_DESC_SERVICE_UNAVAILABLE[]        = "Service Unavailable";
static const char HTTP_STATUS_DESC_HTTP_VERSION_NOT_SUPPORTED[] = "HTTP Version Not Supported";
static const char HTTP_STATUS_UNAUTHORIZED[]                    = "Unauthorized";

Response *create_response(char *http_version, int http_status_code) {
    Response *response;

    response = (Response *) malloc(sizeof(Response));
    memset(response->http_version, 0, 50);
    memset(response->message_body, 0, 5000);

    strcpy(response->http_version, http_version);
    set_response_status_code(response, http_status_code);

    response->n_headers = 0;
    response->c_headers = 1;
    response->headers = (ResponseHeader *) malloc(1 * sizeof(ResponseHeader));

    set_response_message_body_size(response, 0);

    return response;
}

void add_response_header(Response *response, char *key, char *value) {
    size_t capacity;
    ResponseHeader *header;

    if (response->n_headers == response->c_headers) {
        response->headers = (ResponseHeader *) realloc(response->headers, (response->c_headers * 2) * sizeof(ResponseHeader));
        response->c_headers *= 2;
    }

    header = (response->headers) + (response->n_headers);
    strcpy(header->key, key);
    strcpy(header->value, value);

    ++response->n_headers;
}

void add_response_header_i(Response *response, char *key, int value) {
    char value_str[50];
    sprintf(value_str, "%d", value);

    add_response_header(response, key, value_str);
}

void set_response_message_body(Response *response, char *message_body, size_t size) {
    size_t i;
    // TODO: Handle large files
    if (size > MAX_MESSAGE_BODY_SIZE) { size = MAX_MESSAGE_BODY_SIZE; }

    // Works for text and binary files
    memcpy(response->message_body, message_body, size);
    set_response_message_body_size(response, size);
}

void set_response_status_code(Response *response, int status_code) {
    switch (status_code) {
        case HTTP_STATUS_CODE_OK:
            strcpy(response->http_status_description, HTTP_STATUS_DESC_OK);
            break;
        case HTTP_STATUS_CODE_NOT_FOUND:
            strcpy(response->http_status_description, HTTP_STATUS_DESC_NOT_FOUND);
            break;
        case HTTP_STATUS_CODE_LENGTH_REQUIRED:
            strcpy(response->http_status_description, HTTP_STATUS_DESC_LENGTH_REQUIRED);
            break;
        case HTTP_STATUS_CODE_INTERNAL_SERVER_ERROR:
            strcpy(response->http_status_description, HTTP_STATUS_DESC_INTERNAL_SERVER_ERROR);
            break;
        case HTTP_STATUS_CODE_NOT_IMPLEMENTED:
            strcpy(response->http_status_description, HTTP_STATUS_DESC_NOT_IMPLEMENTED);
            break;
        case HTTP_STATUS_CODE_SERVICE_UNAVAILABLE:
            strcpy(response->http_status_description, HTTP_STATUS_DESC_SERVICE_UNAVAILABLE);
            break;
        case HTTP_STATUS_CODE_HTTP_VERSION_NOT_SUPPORTED:
            strcpy(response->http_status_description, HTTP_STATUS_DESC_HTTP_VERSION_NOT_SUPPORTED);
            break;
        case HTTP_STATUS_CODE_UNAUTHORIZED:
            strcpy(response->http_status_description, HTTP_STATUS_CODE_UNAUTHORIZED);
            break;
        default:
            return;
    }
    response->http_status_code = status_code;
}

void set_response_message_body_size(Response *response, size_t size) {
    response->message_body_size = size;
}

void get_response_str(Response *response, char *out_str, bool header_only) {
    size_t i;
    char response_line[512];
    char headers[8192];

    memset(response_line, 0, 512);
    get_response_line_str(response, response_line);

    memset(headers, 0, 8192);
    for (i = 0; i < response->n_headers; ++i) {
        char header_str[512];
        ResponseHeader *header = ((response->headers) + i);
        get_response_header_str(header, header_str);
        strcat(headers, header_str);
    }

    if (header_only) {
        sprintf(out_str, "%s%s\r\n", response_line, headers);
    } else {
        sprintf(out_str, "%s%s\r\n%s", response_line, headers, response->message_body);
    }
}

void destroy_response(Response *response) {
    free(response->headers);
    free(response);
}

static void get_response_line_str(Response *response, char *out_str) {
    sprintf(out_str, "%s %d %s\r\n",
            response->http_version,
            response->http_status_code,
            response->http_status_description
    );
}

static void get_response_header_str(ResponseHeader *header, char *out_str) {
    sprintf(out_str, "%s: %s\r\n",
            header->key,
            header->value
    );
}