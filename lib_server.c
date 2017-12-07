#include "lib_server.h"
#include <stdio.h>
#include "lib_connection.h"
#include "reservsi_mecanics.h"
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include "gams_status.h"
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1


void creat_fork(int client_socket) {
    //int* new_socket = (int*)malloc(sizeof(client_socket));
    connection_base *tmpbase;
    /*pid_t child_pid = fork();
    if (child_pid == 0) {*/
        puts("INFO_:Fork created sucess");
        //child process
        printf("INFO_%d:This is a child thread\n on socket %d", pthread_self(),client_socket);
        tmpbase = init_connection(client_socket);
        if (tmpbase != NULL) {
            printf("INFO_%d_:Client build success,type %x pid %d \n", pthread_self(), tmpbase->client_type, pthread_self());
            Server->cbase[tmpbase->client_type] = *(tmpbase);
            //Server->childpid[tmpbase->client_type] = pthread_self();
            Server->cbase[tmpbase->client_type].handler((void *) (tmpbase));


        } else {
            puts("WARN_:Build failed closing socket");
            close(client_socket);
        }


    } /*else {
        puts("INFO_:This is a parent thread");
    }*/


int8_t init_server() {
    if (!is_init) {
        //init board
        init_game();
        init_game_status();
        //0 - Init server
        is_init =1;
        puts("INFO_:Initialising server");
        Server = (server_struct *) malloc(sizeof(server_struct));
        Server->thread_counter = 0;
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
        int err;
        while (!is_game_over()) {
            printf("INFO_: ...\n");
            //client_socket= accept(Server->socket_desc, (struct sockaddr*)&client,(socklen_t*)&c);
            client_socket = accept(Server->socket_desc, (struct sockaddr *) &client, (socklen_t *) &c);
            if (client_socket != -1) {
                //p- connection accepted
                printf("INFO_:connection accepted on socket %d\n",client_socket);
                //create pthread
                err = pthread_create(&(Server->child[Server->thread_counter]), NULL, &creat_fork,client_socket);
                if (err != 0) {
                    printf("\ncan't create thread :[%s]", strerror(err));
                    return -1;
                }
                Server->thread_counter++;
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
    if (!is_game_over()) {
        set_game_over();
        for(u_int8_t i = 0 ; i < Server->thread_counter ; i++){
            pthread_exit(Server->child[i]);
        }
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
        //end game , destroy board
        end_game();
        destroy_game_status();

        free(Server);
        return 0;
    }
}


