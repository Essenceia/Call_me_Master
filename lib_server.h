#ifndef CPROJ_LIBRARY_H
#define CPROJ_LIBRARY_H

#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include "connection_base_struct.h"
#define MAX_NUM_INCOMING_SOCKETS 3
#define REUSE_OLD_SOCK
typedef struct server_struct{
    int socket_desc;
  struct sockaddr_in srv_addr;
   // socklen_t addr_size;
    int size_sock_addr;
    struct connection_base cbase[MAX_CLIENT_NUMBER];
    pid_t childpid[MAX_CLIENT_NUMBER];
};
static uint8_t is_init = 0;//0 - not init 1 - is init_server
static struct server_struct *Server;

int read_size;
int8_t init_server();
int8_t run_server(); // start listenting
int8_t close_server();// close_server server
void creat_fork(int client_socket);
#endif