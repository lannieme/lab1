#include "request_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#define HEADER_SIZE      8192
#define BODY_SIZE        10000
#define TIME_LENGTH      31
#define FILE_TYPE_LENGTH 50

//set status code
char *STATUS_200 = "200_OK\r\n";
char *STATUS_204 = "204_NO_CONTENT\r\n";
char *STATUS_404 = "404_NOT_FOUND\r\n";
char *STATUS_411 = "411_LENGTH_REQUIRED\r\n";
char *STATUS_500 = "500_INTERNAL_SERVER_ERROR\r\n";
char *STATUS_501 = "501_NOT_IMPLEMENTED\r\n";
char *STATUS_505 = "505_HTTP_VERSION_NOT_SUPPORTED\r\n";

static const char DATE_FORMAT_STR[]  = "%a, %d %b %Y %H:%M:%S GMT";


void get_file_type(char *filename, char *filetype){
	if (strstr(filename, ".html"))
    strcpy(filetype, "text/html");
  else if (strstr(filename, ".css"))
      strcpy(filetype, "text/css");
  else if (strstr(filename, ".js"))
      strcpy(filetype, "application/javascript");
  else if (strstr(filename, ".jpg"))
      strcpy(filetype, "image/jpeg");
  else if (strstr(filename, ".png"))
      strcpy(filetype, "image/png");
  else if (strstr(filename, ".gif"))
      strcpy(filetype, "image/gif");
  else
      strcpy(filetype, "text/plain");
}

void get_file_content(FILE *fp, char *file_contents){
  int i = 0;
  char c;

  if (fp) {
      while((c = fgetc(fp)) != EOF) { 
        file_contents[i++] = c; 
      }
  }
  file_contents[i] = '\0';
}

int get_file_length(char *filename){
  int length = 0;
  fprintf(stderr, "handle line 57 \n"); 
  struct stat st;
  stat(filename, &st);
  fprintf(stderr, "handle line 60 %s \n", st.st_size); 
  length = st.st_size;
  return length;
}

void get_current_time(char *cur_time){
  time_t t;
  t = time(NULL);

  strftime(cur_time, TIME_LENGTH, DATE_FORMAT_STR, gmtime(&t));
}

void get_last_modified(char *filename, char *file_modified){
  struct stat attrib;
  stat(filename, &attrib);

  strftime(file_modified, TIME_LENGTH, DATE_FORMAT_STR, gmtime(&(attrib.st_mtime)));
}


void handle_get(Request *request, char *response, char *ROOT){
  char filename[1024];
  strcpy(filename, ROOT);
  strcat(filename, request->http_uri);

  FILE *fp = fopen(filename, "rb");

  printf("test");


  char header[HEADER_SIZE];

  if (access(request->http_uri, F_OK ) == -1){
    // response for non-existing file
    strcat(header, STATUS_404);
    strcat(header, "Server: liso/1.0\r\n");
    strcat(header, "Connection: close\r\n\r\n");
    printf("The file does not exists \n"); 

  } else {
    // 200 OK
    strcat(header, "HTTP/1.1 ");
    strcat(header, STATUS_200);
    // Date & Time
    char date_time[TIME_LENGTH];
    get_current_time(date_time);
    strcat(header, "Date: ");
    strcat(header, date_time);
    strcat(header, "\r\n");
    // Server
    strcat(header, "Server: Liso/1.0\n");
    // Last Modified
    char last_modified_time[TIME_LENGTH];
    get_last_modified(filename, last_modified_time);
    strcat(header, "Last-Modified: ");
    strcat(header, last_modified_time);
    strcat(header, "\r\n");
    // Content-Length
    size_t file_size = get_file_length(filename);
    strcat(header, "Content-length: ");
    strcat(header, get_file_length(file_size));
    strcat(header,"\r\n");
    // Content-Type
    char file_type[FILE_TYPE_LENGTH];
    get_file_type(filename, file_type);
    strcat(header, "Content-Type: ");
    strcpy(header, file_type);
    strcat(header,"\r\n");
    // Connection 
    // TODO
    // Add "Close"
    strcat(header, "Connection: keep-alive\r\n");
  }

  char file_content[BODY_SIZE];
  get_file_content(fp, file_content);
  fclose(fp);

  strcat(response, header);
  strcat(response, "\r\n");
  strcat(response, file_content);

}

void handle_head(Request *request, char *response, char *ROOT){   
  char filename[1024];
  fprintf(stderr,"root is %s\n", ROOT);
  strcpy(filename, ROOT);
  strcat(filename, request->http_uri);
  fprintf(stderr, "filename %s \n", filename);  
  FILE *fp = fopen(filename, "rb");
  
  char header[HEADER_SIZE];
  char body[BODY_SIZE];

  if (access(request->http_uri, F_OK ) == -1){
    // response for non-existing file
    strcat(header, "HTTP/1.1 ");
    strcat(header, STATUS_404);

    // Date & Time
    char date_time[TIME_LENGTH];
    get_current_time(date_time);
    strcat(header, "Date: ");
    strcat(header, date_time);
    strcat(header, "\r\n");
    // Server
    strcat(header, "Server: Liso/1.0\r\n");
    strcat(header, "Content-length: 8192");
    // strcat(header, HEADER_SIZE);
    strcat(header, "\r\n");

    strcat(header, "Connection: Closed\r\n");
    // strcat(header, "Content-type: text/html\r\n");

    printf("The file does not exists \n"); 
  } else {
    // 200 OK
    strcat(header, "HTTP/1.1 ");
    strcat(header, STATUS_200);
    fprintf(stderr, "handle line 155 %s \n", header); 
    // Date & Time
    char date_time[TIME_LENGTH];
    get_current_time(date_time);
    strcat(header, "Date: ");
    strcat(header, date_time);
    strcat(header, "\r\n");
    fprintf(stderr, "handle line 162 %s \n", header); 
    // Server
    strcat(header, "Server: Liso/1.0\n");
    // Last Modified
    char last_modified_time[TIME_LENGTH];
    get_last_modified(filename, last_modified_time);
    strcat(header, "Last-Modified: ");
    strcat(header, last_modified_time);
    strcat(header, "\r\n");
    fprintf(stderr, "handle line 171 %s \n", header); 
    // Content-Length
    strcat(header, "Content-length: ");
    strcat(header, get_file_length(filename));
    strcat(header,"\r\n");
    fprintf(stderr, "handle line 176 %s \n", header); 
    // Content-Type
    char file_type[FILE_TYPE_LENGTH];
    get_file_type(filename, file_type);
    strcat(header, "Content-Type: ");
    strcpy(header, file_type);
    strcat(header,"\r\n");
    fprintf(stderr, "handle line 183 %s \n", header); 
    // Connection 
    // TODO
    // Add "Close"
    strcat(header, "Connection: keep-alive\r\n");
  }
  strcat(response, header);
}

void handle_post(Request *request, char *response,char *ROOT){
  if (access(request->http_uri, F_OK ) != -1 ) {
    strcat(response, STATUS_200);  
  }
  else {
    strcat(response, STATUS_204);
  }
}

//handle request and point to corresponding method
void handle_request(char *buf,int nbytes,char *response,char *ROOT){
  fprintf(stderr, "handle line 196");

  Request *request = parse(buf, nbytes);
  fprintf(stderr, "handle line 198");

  if (!request) {
    fprintf(stderr, "handle line 202");
    strcat(response, STATUS_500);
    strcat(response, "Server: liso/1.0\r\n");
    strcat(response, "Connection: close\r\n");  
  }

  fprintf(stderr, "206, http version %s\n", request->http_version);
  fprintf(stderr, "206, http method %s \n", request->http_method);
  if (!strcmp(request->http_version , "HTTP/1.1")) {
    strcpy(response, request->http_version);
    strcat(response, " ");
    fprintf(stderr, "handle line 211\n");

    if (!strcmp(request->http_method, "GET")) {
      handle_get(request, response,ROOT);
      fprintf(stderr, "handle line 215\n");
    } 
    else if (!strcmp(request->http_method, "HEAD")) {
      fprintf(stderr, "handle line 218\n");  
      handle_head(request, response,ROOT);
      fprintf(stderr, "handle line 219\n");        
    }
    else if (!strcmp(request->http_method, "POST")) {
      handle_post(request, response,ROOT);        
    }
    else {
      strcat(response, STATUS_501);  
    }
  }
  else {
    strcpy(response, request->http_version);
    strcat(response, " ");
    strcat(response, STATUS_505);
    fprintf(stderr, "handle line 224");
  }
  fprintf(stderr, "handle line 225");
}