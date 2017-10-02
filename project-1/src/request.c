#include "request.h"

#include <stdio.h>
#include <stdlib.h>

void get_header_field_value(Request *request, char key[], char *out_str) {
    return;
}

void destroy_request(Request *request) {
    free(request->headers);
    free(request);
}