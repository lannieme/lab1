/******************************************************************************
* echo_server.c                                                               *
*                                                                             *
* Description: This file contains the C source code for an echo server.  The  *
*              server runs on a hard-coded port and simply write back anything*
*              sent to it by connected clients.  It does not support          *
*              concurrent clients.                                            *
*
* it could supoort concurrent clients now.
* Authors: Athula Balachandran <abalacha@cs.cmu.edu>,                         *
*          Wolf Richter <wolf@cs.cmu.edu>                                     *
*                                                                             *
*******************************************************************************/

#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#define ECHO_PORT 9999
#define BUF_SIZE 4096

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
    ssize_t readret;
    socklen_t cli_size;
    int max_clients = 1024, numbers_listener = 100;
    int sock, client_sock[max_clients];
    int activity, opt = 1, new_sock, c_sock;
    int max_sd, sd;

    struct sockaddr_in addr, cli_addr;
    char buf[BUF_SIZE];

    //set of socket discrptors
    fd_set  readfds;
    fprintf(stdout, "----- Echo Server -----\n");

    /* all networked programs must create a socket */
    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Failed creating socket.\n");
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

    //try to specify maximum of 5 pending connections for the socket
    if (listen(sock, 5)) {
        close_socket(sock);
        fprintf(stderr, "Error listening on socket.\n");
        return EXIT_FAILURE;
    }


    /* finally, loop waiting for input and then write it back */
    while (1)
    {
//       cli_size = sizeof(cli_addr);
//       if ((client_sock = accept(sock, (struct sockaddr *) &cli_addr,
//                                 &cli_size)) == -1)
//       {
//           close(sock);
//           fprintf(stderr, "Error accepting connection.\n");
//           return EXIT_FAILURE;
//       }
//
//       readret = 0;


        //clear the socket set
        FD_ZERO(&readfds);

        //add socket to set
        FD_SET(socket, &readfds);
        max_sd = socket;

        //add child sockets to set
        for(int i = 0; i < max_clients; i++){
            //socket descriptor
            sd = client_sock[i];
            //if valid socket descriptor then add to read list
            if(sd > 0){
                FD_SET(sd, &readfds);
            }
            //highest file descriptor number, need it for the selection function
            if(sd > max_sd){
                max_sd = sd;
            }
        }

        //wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
        activity = select(max_sd + 1 , &readfds , NULL , NULL , NULL);

        if((activity < 0) && (errno!=EINTR)){
            fprinf(stderr, "select error\n");
        }

        //If something happened on the socket, then its an incoming connection
        if (FD_ISSET(socket, &readfds)){
            //accept the incoming connection
            cli_size = sizeof(cli_addr);
            client_sock = accept(sock, (struct sockaddr *) &cli_addr, &cli_size);
            if((new_sock = accept(sock, (struct sockaddr *) &cli_addr, &cli_size)) == -1){
                close(sock);
                fprinf(stedrr, "Error: accepting connection. \n");
                return EXIT_FAILURE;
            }

            for(int i = 0; i<max_clients; i++){
                if(client_sock[i] == 0){
                    client_sock[i] = new_sock;
                    break;
                }
            }

        }

        //add new socket to array of sockets
        for(int i = 0; i < max_clients; i++){
            //if position is empty
            if(client_sock == 0){
                client_sock = accept(sock, (struct sockaddr *) &cli_addr, &cli_size);
                break;
            }
        }
//        readret = 0;

       while((readret = recv(client_sock, buf, BUF_SIZE, 0)) >= 1)
       {
           request = parse_req_str(buf);
           if (request) {
               // Valid request
               printf("Valid request with header:\n%s %s\n",
                      request->http_uri,
                      request->http_method
               );

               for (size_t i = 0; i < request->n_headers; ++i) {
                   RequestHeader rh = request->headers[i];
                   printf("<%s; %s>\n", rh.key, rh.value);
               }
           }

           log_info(logger, cli_addr.sin_addr.s_addr,
                    "Access_Request_Placeholder",
                    200, sizeof(buf));

           if (send(client_sock, buf, readret, 0) != readret)
           {
               close_socket(client_sock);
               close_socket(sock);
               fprintf(stderr, "Error sending to client.\n");
               return EXIT_FAILURE;
           }
           memset(buf, 0, BUF_SIZE);
       } 

       if (readret == -1)
       {
           close_socket(client_sock);
           close_socket(sock);
           fprintf(stderr, "Error reading from client socket.\n");
           return EXIT_FAILURE;
       }

       if (close_socket(client_sock))
       {
           close_socket(sock);
           fprintf(stderr, "Error closing client socket.\n");
           return EXIT_FAILURE;
       }
    }

    close_socket(sock);

    return EXIT_SUCCESS;
}