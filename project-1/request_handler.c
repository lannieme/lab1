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
char *STATUS_200 = "200 OK\r\n";
char *STATUS_204 = "204 No content\r\n";
char *STATUS_401 = "401 Unauthorized\r\n";
char *STATUS_403 = "403 Forbidden\r\n";
char *STATUS_404 = "404 Not Found\r\n";
char *STATUS_411 = "411 Length Required\r\n";
char *STATUS_500 = "500 Internal Server Error\r\n";
char *STATUS_501 = "501 Not Implemented\r\n";
char *STATUS_505 = "505 HTTP Version not supported\r\n";

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

size_t get_file_length(char *filename){
  fprintf(stderr, "handle line 57 \n"); 
  struct stat st;
  stat(filename, &st);
  printf("File name is %s\n", filename);
  return st.st_size;
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

//handle large file
char response_body(char *file_content, size_t size){
  int *max_content_size = 1000;
  if (size > max_content_size){ 
    size = max_content_size;
  }
  char file_content_output[size];
  memcpy(file_content_output, file_content, size);
  return file_content_output;
}

void handle_get(Request *request, char *response, char *ROOT){
  char *filename = malloc(4096);
  strcpy(filename, ROOT);
  strcat(filename, request->http_uri);

  FILE *fp = fopen(filename, "rb");

  char header[HEADER_SIZE];
  char body[BODY_SIZE];
  struct stat statRes;//https://stackoverflow.com/questions/20238042/is-there-a-c-function-to-get-permissions-of-a-file

  if(stat(filename, &statRes) < 0){
    strcat(header, STATUS_404);
    // Server
    strcat(header, "Server: Liso/1.0\r\n");
    // Date & Time
    char date_time[TIME_LENGTH];
    get_current_time(date_time);
    strcat(header, "Date: ");
    strcat(header, date_time);
    strcat(header, "\r\n");
    strcat(header, "Connection: close\r\n");
    strcat(header, "Content-type: text/html\r\n\r\n");

    strcat(body, "<head>\r\n");
    strcat(body, "<title>Error response</title>\r\n");
    strcat(body, "</head>\r\n");
    strcat(body, "<body>\r\n");
    strcat(body, "<h1>Error response</h1>\r\n");
    strcat(body, "<p>Error code 404.</p>\r\n");
    strcat(body, "<p>Message: Not Found.</p>\r\n");
    strcat(body, "<p>Error code explanation: 404 = Nothing matches the given URI.</p>\r\n");
    strcat(body, "</body>");
    
    strcat(response, header);
    strcat(response, body);

    
    printf("The file does not exists \n"); 

  } else if ((statRes.st_mode & S_IRUSR) == 0 ) {
    //no permission to read
    strcat(header, STATUS_403);
    // Server
    strcat(header, "Server: Liso/1.0\r\n");
    // Date & Time
    char date_time[TIME_LENGTH];
    get_current_time(date_time);
    strcat(header, "Date: ");
    strcat(header, date_time);
    strcat(header, "\r\n");
    strcat(header, "Connection: close\r\n");
    strcat(header, "Content-type: text/html\r\n\r\n");

    strcat(body, "<head>\r\n");
    strcat(body, "<title>Error response</title>\r\n");
    strcat(body, "</head>\r\n");
    strcat(body, "<body>\r\n");
    strcat(body, "<h1>Error response</h1>\r\n");
    strcat(body, "<p>Error code 403.</p>\r\n");
    strcat(body, "<p>Message: Forbidden.</p>\r\n");
    strcat(body, "<p>Error code explanation: 403 = Permission denied.</p>\r\n");
    strcat(body, "</body>");
    
    strcat(response, header);
    strcat(response, body);
  } 
    else {
    // 200 OK
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
    char str[256];
    sprintf(str, "%ld", file_size);
    fprintf(stderr, "The length here is!!!%s\n", str);
    strcat(header, "Content-length: ");
    strcat(header, str);
    strcat(header,"\r\n");
    // Content-Type
    char file_type[FILE_TYPE_LENGTH];
    get_file_type(filename, file_type);
    strcat(header, "Content-Type: ");
    strcat(header, file_type);
    strcat(header,"\r\n");
    // Connection 
    strcat(header, "Connection: keep-alive\r\n");

    // char file_content[BODY_SIZE];
    char *file_content = malloc(file_size * sizeof(char));
    get_file_content(fp, file_content);



    fclose(fp);

    strcat(response, header);
    strcat(response, "\r\n");
    strcat(response, file_content);
    strcat(response, "\r\n\r\n");
    free(file_content);
  }
}

void handle_head(Request *request, char *response, char *ROOT){   
  char *filename = malloc(4096);
  fprintf(stderr,"root is %s\n", ROOT);
  strcpy(filename, ROOT);
  strcat(filename, request->http_uri);
  fprintf(stderr, "filename %s \n", filename);  
  FILE *fp = fopen(filename, "rb");
  
  char header[HEADER_SIZE];
  char body[BODY_SIZE];

  struct stat statRes;//https://stackoverflow.com/questions/20238042/is-there-a-c-function-to-get-permissions-of-a-file

  // if (access(filename, F_OK ) == -1){
  if(stat(filename, &statRes) < 0){
    strcpy(header, STATUS_404);
    // Server
    strcat(header, "Server: Liso/1.0\r\n");
    // Date & Time
    char date_time[TIME_LENGTH];
    get_current_time(date_time);
    strcat(header, "Date: ");
    strcat(header, date_time);
    strcat(header, "\r\n");
    strcat(header, "Connection: close\r\n");
    strcat(header, "Content-type: text/html\r\n\r\n");

    // strcat(body, "<head>\r\n");
    // strcat(body, "<title>Error response</title>\r\n");
    // strcat(body, "</head>\r\n");
    // strcat(body, "<body>\r\n");
    // strcat(body, "<h1>Error response</h1>\r\n");
    // strcat(body, "<p>Error code 404.</p>\r\n");
    // strcat(body, "<p>Message: Not Found.</p>\r\n");
    // strcat(body, "<p>Error code explanation: 404 = Nothing matches the given URI.</p>\r\n");
    // strcat(body, "</body>");
    
    strcat(response, header);
    // strcat(response, body);

    
    printf("The file does not exists \n"); 
  } else if ((statRes.st_mode & S_IRUSR) == 0 ) {
    //no permission to read
    strcpy(header, STATUS_403);
    // Server
    strcat(header, "Server: Liso/1.0\r\n");
    // Date & Time
    char date_time[TIME_LENGTH];
    get_current_time(date_time);
    strcat(header, "Date: ");
    strcat(header, date_time);
    strcat(header, "\r\n");
    strcat(header, "Connection: close\r\n");
    strcat(header, "Content-type: text/html\r\n\r\n");

    // strcat(body, "<head>\r\n");
    // strcat(body, "<title>Error response</title>\r\n");
    // strcat(body, "</head>\r\n");
    // strcat(body, "<body>\r\n");
    // strcat(body, "<h1>Error response</h1>\r\n");
    // strcat(body, "<p>Error code 403.</p>\r\n");
    // strcat(body, "<p>Message: Forbidden.</p>\r\n");
    // strcat(body, "<p>Error code explanation: 403 = Permission denied.</p>\r\n");
    // strcat(body, "</body>");
    
    strcat(response, header);
    // strcat(response, body);
  } 
    else{
    // 200 OK
    fprintf(stderr, "handle line 274  %s ih \n", header);   
    strcpy(header, STATUS_200);
    fprintf(stderr, "handle line 275  %s hi \n", header); 
    // Date & Time
    char date_time[TIME_LENGTH];
    get_current_time(date_time);
    strcat(header, "Date: ");
    strcat(header, date_time);
    strcat(header, "\r\n");
    // Server
    strcat(header, "Server: Liso/1.0\r\n");
    // Last Modified
    char last_modified_time[TIME_LENGTH];
    get_last_modified(filename, last_modified_time);
    strcat(header, "Last-Modified: ");
    strcat(header, last_modified_time);
    strcat(header, "\r\n");
    // Content-Length
    size_t file_size = get_file_length(filename);
    char str[256];
    sprintf(str, "%ld", file_size);
    fprintf(stderr, "The length here is!!!%s\n", str);
    strcat(header, "Content-length: ");
    strcat(header, str);
    strcat(header,"\r\n");
    // Content-Type
    char file_type[FILE_TYPE_LENGTH];
    get_file_type(filename, file_type);
    strcat(header, "Content-Type: ");
    strcat(header, file_type);
    strcat(header,"\r\n");
    // Connection 
    strcat(header, "Connection: keep-alive\r\n\r\n");
    fprintf(stderr, "handle line 302  %s 302\n",header); 
    strcat(response, header);
    fprintf(stderr, "handle line 305  %s 305\n",response); 
  }
  
}

void handle_post(Request *request, char *response,char *ROOT){
  char *filename = ROOT;
  strcat(filename, request->http_uri);
  if (access(filename, F_OK ) != -1 ) {
    strcat(response, STATUS_200);
    strcat(response, "Server: liso/1.0\r\n");
    char date_time[TIME_LENGTH];
    get_current_time(date_time);
    strcat(response, "Date: ");
    strcat(response, date_time);
    strcat(response, "\r\n");
    strcat(response, "Connection: close\r\n");  
    strcat(response, "Content-Type: text/html\r\n\r\n");
  }
  else {
    strcat(response, STATUS_401);
    strcat(response, "Server: liso/1.0\r\n");
    char date_time[TIME_LENGTH];
    get_current_time(date_time);
    strcat(response, "Date: ");
    strcat(response, date_time);
    strcat(response, "\r\n");
    strcat(response, "Connection: close\r\n");  
    strcat(response, "Content-Type: text/html\r\n\r\n");
  }
}

//handle request and point to corresponding method
void handle_request(char *buf,int nbytes,char *response,char *ROOT){
  // printf("Buffer: %s\n", buf);
  Request *request = parse(buf, nbytes);

  if (!request) {
    fprintf(stderr, "line 346: request handler.\n");
    strcpy(response, "HTTP/1.1");
    strcat(response, " ");
    strcat(response, STATUS_500);
    strcat(response, "Server: liso/1.0\r\n");
    char date_time[TIME_LENGTH];
    get_current_time(date_time);
    strcat(response, "Date: ");
    strcat(response, date_time);
    strcat(response, "\r\n");
    strcat(response, "Connection: close\r\n\r\n");  
    return;
  }

  if (!strcmp(request->http_version , "HTTP/1.1")) {
    //set http version for all
    strcpy(response, request->http_version);
    strcat(response, " ");

    if (!strcmp(request->http_method, "GET")) {
      handle_get(request, response, ROOT);
    } 
    else if (!strcmp(request->http_method, "HEAD")) {
      handle_head(request, response, ROOT);       
    }
    else if (!strcmp(request->http_method, "POST")) {
      handle_post(request, response, ROOT);        
    }
    else {//not implemented method
      strcpy(response, request->http_version);
      strcat(response, " ");
      strcat(response, STATUS_501);
      strcat(response, "Server: liso/1.0\r\n");
      char date_time[TIME_LENGTH];
      get_current_time(date_time);
      strcat(response, "Date: ");
      strcat(response, date_time);
      strcat(response, "\r\n");
      strcat(response, "Connection: close\r\n");  
      strcat(response, "Content-Type: text/html\r\n\r\n");
      
      strcat(response, "<head>\r\n");
      strcat(response, "<title>Error response</title>\r\n");
      strcat(response, "</head>\r\n");
      strcat(response, "<body>\r\n");
      strcat(response, "<h1>Error response</h1>\r\n");
      strcat(response, "<p>Error code 501.</p>\r\n");
      strcat(response, "<p>Message: Unsupported method ('HET').</p>\r\n");
      strcat(response, "<p>Error code explanation: 501 = Server does not support this operation.</p>\r\n");
      strcat(response, "</body>");   
    }
  }
  else {//not supported HTTP VERSION
    strcpy(response, request->http_version);
    strcat(response, " ");
    strcat(response, STATUS_505);
    strcat(response, "Server: liso/1.0\r\n");
    char date_time[TIME_LENGTH];
    get_current_time(date_time);
    strcat(response, "Date: ");
    strcat(response, date_time);
    strcat(response, "\r\n");
    strcat(response, "Connection: close\r\n"); 
    strcat(response, "Content-Type: text/html\r\n\r\n");

    strcat(response, "<head>\r\n");
    strcat(response, "<title>Error response</title>\r\n");
    strcat(response, "</head>\r\n");
    strcat(response, "<body>\r\n");
    strcat(response, "<h1>Error response</h1>\r\n");
    strcat(response, "<p>Error code 505.</p>\r\n");
    strcat(response, "<p>Message: HTTP Version not supported.</p>\r\n");
    strcat(response, "<p>Error code explanation: 505 = Cannot fulfill request..</p>\r\n");
    strcat(response, "</body>");    
  }
}