#include "lib_server.h"
#include <stdio.h>
#include "lib_connection.h"
#include "reservsi_mecanics.h"
#include <string.h>
#include <signal.h>
#include <unistd.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

void kill_handler(int killid) {
    printf("WARN_:kill handler called , process will be killed with message %d\n", killid);
    close_server();
}

void client_kill_handler(int killid) {
    printf("WARN_:Client kill handler, child process will be killes with message %d\n", killid);
    for (int i = 0; i < MAX_CLIENT_NUMBER; i++) {
        destroy_connection_base(&(Server->cbase[i]));
    }
}

void warn_server() {
    printf("WARN_:Nothing happening server is gonna die of bordom\n");
}

void creat_fork(int client_socket) {
    //int* new_socket = (int*)malloc(sizeof(client_socket));
    int new_socket = client_socket;
    connection_base *tmpbase;
    pid_t child_pid = fork();
    if (child_pid == 0) {
        puts("INFO_:Fork created sucess");
        //child process
        printf("INFO_%d:This is a child thread\n", getpid());
        tmpbase = init_connection(client_socket);
        if (tmpbase != NULL) {
            printf("INFO_%d_:Client build success,type %x pid %d \n", getpid(), tmpbase->client_type, getpid());
            Server->cbase[tmpbase->client_type] = *(tmpbase);
            Server->childpid[tmpbase->client_type] = getpid();
            Server->cbase[tmpbase->client_type].handler((void *) (tmpbase));


        } else {
            puts("WARN_:Build failed closing socket");
            close(client_socket);
        }


    } else {
        puts("INFO_:This is a parent thread");
    }
}

int8_t init_server() {
    int buid_result;
    if (!is_init) {
        //init board
        init_game();
        //0 - Init server
        is_init =1;
        puts("INFO_:Initialising server");
        signal(SIGINT, kill_handler);
        GAME_OVER = 0;
        Server = (server_struct *) malloc(sizeof(server_struct));
        Server->socket_desc = socket(AF_INET, SOCK_STREAM, 0);
        if (Server->socket_desc == -1) {
            printf("Server :: ERROR : unable to create socket. \n", 0);
            return -1;
        }
        printf("Server :: Socket created. \n");
        int enable = 1;
        if (setsockopt(Server->socket_desc, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
            puts("ERRO_:Unable to make socket reusable");
            exit(-1);
        }
        Server->srv_addr.sin_addr.s_addr = INADDR_ANY; // Address to accept any incoming messages
        Server->srv_addr.sin_family = AF_INET;
        Server->srv_addr.sin_port = htons(8888); // convert between host and network byte order.


        // Bind
        if (bind(Server->socket_desc, (struct sockaddr *) &Server->srv_addr, sizeof(Server->srv_addr)) < 0) {
            perror("Server :: Unable to bind. Error ");
            return -1;
        }
        printf("Server :: Bind complete.\n");
        //Start to listen
        if(listen(Server->socket_desc, MAX_CLIENT_NUMBER) == SOCKET_ERROR)
        {
            perror("Server listen :");
            return -1;
        }
        //Server->size_sock_addr = sizeof(struct sockaddr_in);
        //p- Waiting of incomming transmissions
        printf("INFO_:Has been intialised , ready to be run\n");
        return 0;

    } else {
        puts("INFO_:server already initialised");
        return 0;
    }
}

int8_t run_server() {
    if(is_init) {
        puts("INFO_:Startting server\n");
        int client_socket;
        struct sockaddr_in client;
        int c = sizeof(client);
        while (!GAME_OVER) {
            printf("INFO_: ...\n");
            //client_socket= accept(Server->socket_desc, (struct sockaddr*)&client,(socklen_t*)&c);
            client_socket = accept(Server->socket_desc, (struct sockaddr *) &client, (socklen_t *) &c);
            if (client_socket != -1) {
                //p- connection accepted
                printf("INFO_:connection accepted\n");
                //create pthread
                creat_fork(client_socket);
                //client_socket = -10; //reset client socket
            } else {

                perror("Server");
                sleep(10);
                printf("WARN_:accept failed with error %d\n", client_socket);

                //return -2;
            }
        }
        close_server();
    }else puts("ERRO_:Server not initialised\n");
    return 0;

}

/*
 * Close server ,
 * to do list :
 *  -close sockets x
 *  -free connection base ~ we have sent a kill switch now need to implement
 *  -destroy board x
 */
int8_t close_server() {
    if (GAME_OVER == 0) {
        GAME_OVER = 1;
        for (int i = 0; i < MAX_CLIENT_NUMBER; i++) {
            //kill all child threads
            if (Server->cbase[i].alive == ALIVE) {
                //kill(Server->childpid[i],SIGTERM);
                destroy_connection_base(&Server->cbase[i]);
            }
        }
        puts("INFO_:Closing server");
        puts("INFO_:Closing server socket");
        close(Server->socket_desc);
        //destroy child processes
        kill(0, SIGINT);
        //end game , destroy board
        end_game();
        free(Server);
        return 0;
    }
}


