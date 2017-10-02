//
// Created by Huai-Che Lu on 2/27/17.
//

#include "parser.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../request.h"

#define MAX_HEADER_LEN  8192

static bool has_valid_header(char *req_str, char *header, size_t size);

Request *parse_req_str(char *req_str) {
    char header[8192];
    Request *request;
    int capacity = 1;

    if (!has_valid_header(req_str, header, MAX_HEADER_LEN)) {
        // TODO: Handle mal-formatted header
        printf("Mal-formatted header\n");
        return NULL;
    }

    request = (Request *) malloc(sizeof(Request));
    request->n_headers = 0;
    request->c_headers = 1;
    request->headers = (RequestHeader *) malloc(capacity * sizeof(RequestHeader));

    set_parsing_options(header, strlen(header), request);


    if (yyparse() == SUCCESS) {
        return request;
    }

    return NULL;
}

bool has_valid_header(char *req_str, char *header, size_t size) {
    enum State {
        START = 0, CR, CRLF,
        CRLFCR, CRLFCRLF
    };

    int i, state;
    char c;

    i = 0;
    state = START;

    while (state != CRLFCRLF) {
        char expected = '\0';

        if (i == size || req_str[i] == '\0') {
            break;
        }

        c = *(header++) = req_str[i++];

        switch (state) {
        case START:
        case CRLF:
            expected = '\r';
            break;
        case CR:
        case CRLFCR:
            expected = '\n';
            break;
        }

        if (c == expected) {
            ++state;
        } else {
            state = START;
        }
    }

    return state == CRLFCRLF;
}


