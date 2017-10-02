//
// Created by Huai-Che Lu on 2/28/17.
//

#include "request_handler.h"

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

#include "../request.h"
#include "../response.h"

#define TIME_STR_LENGTH 31

static const char MIME_TYPE_UNKOWN[] = "application/unknown";
static const char MIME_TYPE_HTML[]   = "text/html";
static const char MIME_TYPE_CSS[]    = "text/css";
static const char MIME_TYPE_JPEG[]   = "image/jpeg";
static const char MIME_TYPE_PNG[]    = "image/png";
static const char MIME_TYPE_GIF[]    = "image/gif";

static const char HTTP_VER[]  = "HTTP/1.1";
static const char DATE_FORMAT_STR[]  = "%a, %d %b %Y %H:%M:%S GMT";

static void get_current_time(char *out_str);
static int get_file_length(char *file_path);
static void get_file_type(char *file_path, char *out_str);
static void get_file_last_modified_time(char *file_path, char *out_str);
static void get_file_content(FILE *fp, char *out_str);
static Response *calculate_size(Request *request);

Response *handle_request(Request *request) {

    Response *response;
    response = create_response((char *) HTTP_VER, HTTP_STATUS_CODE_OK);

    /**
     * General Headers
     * From the list below you need to implement at least Connection and Date
     * right now.  For Date lookup the strftime() function.
     */

    // Add Connection TODO: Change to close for some cases
    add_response_header(response, "Connection", "keep-alive");

    // Add Date
    char current_time[TIME_STR_LENGTH];
    get_current_time(current_time);
    add_response_header(response, "Date", current_time);

    if (!request) {
        set_response_status_code(response, HTTP_STATUS_CODE_INTERNAL_SERVER_ERROR);
        return response;
    }

    if (request->method_type == UNIMPLEMENTED) {
        set_response_status_code(response, HTTP_STATUS_CODE_NOT_IMPLEMENTED);
        return response;
    }

    if (strcmp(request->http_version, "HTTP/1.1") != 0) {
        set_response_status_code(response, HTTP_STATUS_CODE_HTTP_VERSION_NOT_SUPPORTED);
        return response;
    }

    // //permission deny
    // if(request->){
    //     set_response_status_code(response, HTTP_STATUS_UNAUTHORIZED);
    //     return response;
    // }

    char file_path[1024];
    sprintf(file_path, ".%s", request->http_uri);

    if (access(file_path, R_OK) == -1) {
        set_response_status_code(response, HTTP_STATUS_CODE_NOT_FOUND);
        return response;
    }

    FILE *fp = fopen(file_path, "rb");
    response = create_response(request->http_version, HTTP_STATUS_CODE_OK);

    /**
     * Entity Headers
     * From the list below you need to implement at a minimum Content-Length,
     * Content-Type, and Last-Modified.
     */

    // Add Content-Length
    size_t file_len = get_file_length(file_path);
    add_response_header_i(response, "Content-Length", file_len);

    // Add Content-Type
    char file_type[50];
    get_file_type(file_path, file_type);
    add_response_header(response, "Content-Type", file_type);

    // Add Last-Modified
    char last_modified_time[TIME_STR_LENGTH];
    get_file_last_modified_time(file_path, last_modified_time);
    add_response_header(response, "Last-Modified", last_modified_time);

    if (request->method_type == HEAD) {
        return response;
    }

    // Access actual file content
    char content[10000];
    get_file_content(fp, content);
    fclose(fp);

    set_response_message_body(response, content, file_len);
    return response;
}

static int get_file_length(char *file_path) {
    struct stat st;
    stat(file_path, &st);
    return st.st_size;
//
//    int file_length;
//    fseek(fp, 0, SEEK_END);
//    file_length = ftell(fp);
//    fseek(fp, 0, SEEK_SET);
//    return file_length;
}

static void get_current_time(char *out_str) {
    time_t t;
    t = time(NULL);

    strftime(out_str, TIME_STR_LENGTH, DATE_FORMAT_STR, gmtime(&t));
}

static void get_file_last_modified_time(char *file_path, char *out_str) {
    struct stat attrib;
    stat(file_path, &attrib);

    strftime(out_str, TIME_STR_LENGTH, DATE_FORMAT_STR, gmtime(&(attrib.st_mtime)));
}

static void get_file_type(char *file_path, char *out_str) {
    char *ext;
    ext = strrchr(file_path, '.');
    if (!ext) {
        /* no extension */
        printf("No ext\n");
        strcpy(out_str, "application/unknown");
        return;
    }

    ++ext;
    char *tmp = ext;
    for ( ; *tmp; ++tmp) *tmp = tolower(*tmp);

    if (strcmp(ext, "html") == 0 || strcmp(ext, "htm") == 0) {
        strcpy(out_str, MIME_TYPE_HTML);
        return;
    }

    if (strcmp(ext, "css") == 0) {
        strcpy(out_str, MIME_TYPE_CSS);
        return;
    }

    if (strcmp(ext, "png") == 0) {
        strcpy(out_str, MIME_TYPE_PNG);
        return;
    }

    if (strcmp(ext, "jpeg") == 0 || strcmp(ext, "jpg") == 0 || strcmp(ext, "jpe") == 0) {
        strcpy(out_str, MIME_TYPE_JPEG);
        return;
    }

    if (strcmp(ext, "gif") == 0) {
        strcpy(out_str, MIME_TYPE_GIF);
        return;
    }

    strcpy(out_str, MIME_TYPE_UNKOWN);
}

static void get_file_content(FILE *fp, char *out_str) {
    int i = 0;
    char c;

    if (fp) {
        while((c = fgetc(fp)) != EOF) { out_str[i++] = c; }
    }
    out_str[i] = '\0';
}