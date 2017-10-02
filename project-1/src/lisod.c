/******************************************************************************
* echo_server.c                                                               *
*                                                                             *
* Description: This file contains the C source code for an echo server.  The  *
*              server runs on a hard-coded port and simply write back anything*
*              sent to it by connected clients.  It does not support          *
*              concurrent clients.                                            *
*                                                                             *
* Authors: Athula Balachandran <abalacha@cs.cmu.edu>,                         *
*          Wolf Richter <wolf@cs.cmu.edu>                                     *
*                                                                             *
*******************************************************************************/

#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "logger/logger.h"
#include "parser/parser.h"
#include "request.h"
#include "response.h"
#include "request_handler/request_handler.h"

#define ECHO_PORT       9999
#define BUF_SIZE        8192
#define MAX_CLIENT_NUM  1024

#define MAX(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

void add_client_sock(int client_sock, int *client_socks, size_t size);

int close_socket(int sock)
{
    if (close(sock))
    {
        fprintf(stderr, "Failed closing socket.\n");
        return 1;
    }
    return 0;
}

int main(int argc, char* argv[])
{
    int opt = 1;
    int sock, client_sock;
    int i;
    int client_socks[MAX_CLIENT_NUM];
    ssize_t readret;
    socklen_t cli_size;
    struct sockaddr_in addr, cli_addr;
    char buf[BUF_SIZE];
    logger_t *logger;
    Request *request;
    Response *response;

    fd_set read_fds;
    int fd_max;

    FD_ZERO(&read_fds);

    fprintf(stdout, "----- Liso Server -----\n");

    /* all networked programs must create a socket */
    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Failed creating socket.\n");
        return EXIT_FAILURE;
    }

    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
    {
        fprintf(stderr, "Server-setsockopt() error");
        return EXIT_FAILURE;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(ECHO_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    /* servers bind sockets to ports---notify the OS they accept connections */
    if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)))
    {
        close_socket(sock);
        fprintf(stderr, "Failed binding socket.\n");
        return EXIT_FAILURE;
    }


    if (listen(sock, 30))
    {
        close_socket(sock);
        fprintf(stderr, "Error listening on socket.\n");
        return EXIT_FAILURE;
    }

    for (i = 0; i < MAX_CLIENT_NUM; ++i) {
        client_socks[i] = 0;
    }

    logger = create_logger();
    bind_log_file(logger, "server.log");
    memset(buf, 0, BUF_SIZE);

    /* finally, loop waiting for requests */
    while (1)
    {
        FD_ZERO(&read_fds);
        FD_SET(sock, &read_fds);
        fd_max = sock;

        for (i = 0; i < MAX_CLIENT_NUM; ++i) {
            client_sock = client_socks[i];
            if (client_sock != 0) {
                FD_SET(client_sock, &read_fds);
                fd_max = MAX(fd_max, client_sock);
            }
        }

        if (select(fd_max + 1, &read_fds, NULL, NULL, NULL) < 0)
        {
            fprintf(stderr, "Server-select() error.\n");
            return EXIT_FAILURE;
        }

        if (FD_ISSET(sock, &read_fds)) {
            // Accept new connections
            cli_size = sizeof(cli_addr);
            if ((client_sock = accept(sock, (struct sockaddr *) &cli_addr,
                                      &cli_size)) == -1) {
                close(sock);
                fprintf(stderr, "Error accepting connection.\n");
                return EXIT_FAILURE;
            }

            add_client_sock(client_sock, &client_socks, MAX_CLIENT_NUM);
        }


        for (i = 0; i < MAX_CLIENT_NUM; ++i) {
            client_sock = client_socks[i];
            readret = 0;
            if (FD_ISSET(client_sock, &read_fds)) {
                if ((readret = recv(client_sock, buf, BUF_SIZE - 1, 0)) == 0) {
                    close_socket(client_sock);
                    client_socks[i] = 0;
                    // TODO: Log addr
                    fprintf(stderr, "client socket %d disconnected\n", client_sock);
                } else {
                    ssize_t bytes;

                    printf("Get Request!!!\n");
                    request = parse_req_str(buf);
                    printf("Finish parsing!!!\n");
                    response = handle_request(request);
                    printf("Finish handling request!!!\n");

                    get_response_str(response, buf, true);
                    bytes = strlen(buf) + response->message_body_size;
                    printf("Sending %d + %d = %d bytes\n",
                           strlen(buf),
                           response->message_body_size,
                           bytes
                    );

                    // TODO: Fix sending size
                    log_info(logger, cli_addr.sin_addr.s_addr,
                             request->http_uri,
                             response->http_status_code, strlen(buf));

                    get_response_str(response, buf, false);
                    // TODO: Fix sending size
                    if (send(client_sock, buf, bytes, 0) != bytes) {
                        close_socket(client_sock);
                        client_socks[i] = 0;
                        fprintf(stderr, "Error sending to client.\n");
                    }
                    memset(buf, 0, BUF_SIZE);
                    request = NULL;
                    response = NULL;
                    if (request) {
                        destroy_request(request);
                        request = NULL;
                    }
                    if (response) {
                        destroy_response(response);
                        response = NULL;
                    }
                }
            }
        }
    }

    for (i = 0; i < MAX_CLIENT_NUM; ++i) {
        client_sock = client_socks[i];
        close_socket(client_sock);
    }

    close_socket(sock);
    terminate_logger(logger);

    return EXIT_SUCCESS;
}

void add_client_sock(int client_sock, int *client_socks, size_t size) {
    size_t i;

    for (i = 0; i < size; ++i) {
        if (*(client_socks + i) == 0) {
            *(client_socks + i) = client_sock;
            return;
        }
    }
}
