/******************************************************************************
* echo_server.c                                                               *
*                                                                             *
* Description: Modification                                    *
*                                                                             *
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
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "log.h"
#include "request_handler.h"

// #define ECHO_PORT 9999
#define BUF_SIZE 8192
#define MAX_NO_CLIENT 1024

FILE *fp;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int close_socket(int sock)
{
    if (close(sock))
    {
        fprintf(stderr, "Failed closing socket.\n");
        return 1;
    }
    fprintf(stderr, "Closed socket.\n");
    return 0;
}

int main(int argc, char* argv[])
{   
   //TO-DO: open log file
  int sock, client_sock; // listening socket descriptor
  socklen_t cli_size;
  struct sockaddr_in addr, cli_addr; // client address
  char buf[8192]; // buffer for client data
  int nbytes;
  int ECHO_PORT;

  ECHO_PORT = atoi(argv[1]);
  char *server_log = argv[3];
  char *ROOT = argv[5];

  fp = open_log(server_log);

  fd_set master;    // master file descriptor list
  fd_set read_fds;  // temp file descriptor list for select()
  int fdmax;        // maximum file descriptor number

  char remoteIP[INET6_ADDRSTRLEN];
  int i;

  Log(fp, "----- Start Liso Server ----- \n");
  
  /* all networked programs must create a socket */
  if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
  {
      Log(fp, "Failed creating socket.\n");
      return EXIT_FAILURE;
  }

  Log(fp, "Server socket successfully created.\n");

  addr.sin_family = AF_INET;
  
  addr.sin_port = htons(ECHO_PORT);
  addr.sin_addr.s_addr = INADDR_ANY;

  //TO-do: logfile check failure
  FD_ZERO(&master);    // clear the master and temp sets
  FD_ZERO(&read_fds);

  /* servers bind sockets to ports---notify the OS they accept connections */
  if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)))
  {
      close_socket(sock);
      Log(fp, "Failed binding socket.\n");
      return EXIT_FAILURE;
  }

  if (listen(sock, 5))
  {
      close_socket(sock);
      Log(fp, "Error listening on socket.\n");
      return EXIT_FAILURE;
  }
  
  // add the listener to the master set
  FD_SET(sock, &master);

  // keep track of the biggest file descriptor
  fdmax = sock; // so far, it's this one
  /* finally, loop waiting for input and then write it back */
  while (1)
  {
    read_fds = master; // copy it
    if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
        perror("select");
        Log(fp, "ERROR: not able to select\n");
        exit(4);
    }

    // run through the existing connections looking for data to read
    for(i = 0; i <= fdmax; i++) {
        if (FD_ISSET(i, &read_fds)) { // we got one!!
            if (i == sock) {
                // handle new connections
                cli_size = sizeof(cli_addr);
                client_sock = accept(sock,
                    (struct sockaddr *)&cli_addr,
                    &cli_size);

                if (client_sock == -1) {
                    perror("accept");
                    Log(fp, "ERROR: not able to accept connection\n");
                } else {
                    FD_SET(client_sock, &master); // add to master set
                    if (client_sock > fdmax) {    // keep track of the max
                        fdmax = client_sock;

                    }
                    printf("selectserver: new connection from %s on "
                        "socket %d\n",
                        inet_ntop(cli_addr.sin_family,
                            get_in_addr((struct sockaddr*)&cli_addr),
                            remoteIP, INET6_ADDRSTRLEN),
                        client_sock);
                    fprintf(stderr, "selectserver: new connection from %s on "
                        "socket %d\n",
                        inet_ntop(cli_addr.sin_family,
                            get_in_addr((struct sockaddr*)&cli_addr),
                            remoteIP, INET6_ADDRSTRLEN),
                        client_sock);
                }
            } else {
                // handle data from a client
                fprintf(stderr, "line 162");
                char* response = malloc(sizeof(char)*10000);
                

                if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                    // got error or connection closed by client
                    if (nbytes == 0) {
                        // connection closed
                        fprintf(stderr, "selectserver: socket %d hung up\n", i);
                        Log(fp, "selectserver: socket %d hung up\n");
                    } else {
                        perror("recv");
                        fprintf(stderr, "ERROR: not able to receive data from socket %d\n",i );
                        Log(fp, "ERROR: not able to receive data from socket %d\n");
                    }
                    close(i); // bye!
                    FD_CLR(i, &master); // remove from master set
                } else { //data ready to proceed

                    handle_request(buf,nbytes,response,ROOT);
                    memset(buf,0,nbytes);

                    fprintf(stderr, "response here : %s\n",response );
                    int status = send(i, response, strlen(response), 0);
                    if (status  == -1) {
                      perror("send");
                      fprintf(stderr, "ERROR：not able to sent data to client");
                      Log(fp, "ERROR：not able to sent data to client\n");
                    }else{
                      fprintf(stderr, "Sent response!");
                      Log(fp, "Sent response!\n");
                    }

                    free(response);


                    // close(i); // bye!
                    // FD_CLR(i, &master); // remove from master set
                }
            } // END handle data from client
        } // END got new incoming connection
    } // END looping through file descriptors

  }
  close_log(fp);
  close_socket(sock);
  return EXIT_SUCCESS;
}
