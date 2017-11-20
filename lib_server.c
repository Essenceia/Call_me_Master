#include "lib_server.h"

#include "lib_server.h"
#include <stdlib.h>
#include <stdio.h>
#include "lib_connection.h"
#include "board_handler.h"
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <zconf.h>
#include <signal.h>
#include "board_handler.h"
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
void kill_handler(int killid){
    printf("WARN_:kill handler called , process will be killed with message %d\n",killid);
    close_server();
}
void client_kill_handler(int killid){
    printf("WARN_:Client kill handler, child process will be killes with message %d\n",killid);
    for(int i = 0 ; i < MAX_CLIENT_NUMBER; i++){
        destroy_connection_base(&(Server->cbase[i]));
    }
}
void warn_server(){
    printf("WARN_:Nothing happening server is gonna die of bordom\n");
}

void creat_fork(int client_socket){
    //int* new_socket = (int*)malloc(sizeof(client_socket));
    int new_socket = client_socket;
    struct connection_base *tmpbase;
    pid_t child_pid = fork();
    if(child_pid==0){
        puts("INFO_:Fork created sucess");
            //child process
            printf("INFO_%d:This is a child thread\n",getpid());
            tmpbase = init_connection(client_socket);
            if(tmpbase!=NULL){
                printf("INFO_%d_:Client build success,type %x pid %d \n",getpid(),tmpbase->client_type,getpid());
                Server->cbase[tmpbase->client_type] = *(tmpbase);
                Server->childpid[tmpbase->client_type]= getpid();
                Server->cbase[tmpbase->client_type].handler((void*)(tmpbase),client_socket);
            }else{
                puts("WARN_:Build failed closing socket");
                close(client_socket);
            }


    }else{
        puts("INFO_:This is a parent thread");
    }
}
int8_t init_server(){
    int buid_result;
    if(!is_init){
        //0 - Init server
        puts("INFO_:Initialising server");
        signal(SIGINT,kill_handler);
        GAME_OVER=0;

        //1- Create socket
        Server = (struct server_struct *)malloc(sizeof(struct server_struct));
        Server->socket_desc = socket(AF_INET, SOCK_STREAM , 0);

        if(Server->socket_desc == INVALID_SOCKET){
            //p- printf("Could not create socket");
            puts("ERRO_:Could not create socket");
            exit(-1);
        }else{
            //p- puts("Socket created");
            puts("INFO_:socket created sucess");
        }
        //2- creat interface on server
        memset(&(Server->srv_addr) ,0,sizeof(Server->srv_addr));//fill with zero
        Server->srv_addr.sin_family = AF_INET;
#ifndef REUSE_OLD_SOCK
        Server->srv_addr.sin_addr.s_addr = INADDR_ANY;
#else
        Server->srv_addr.sin_addr.s_addr=htons( INADDR_ANY );
#endif
        Server->srv_addr.sin_port = htons(8888);
        buid_result = bind(Server->socket_desc,(struct sockaddr *)&(Server->srv_addr),sizeof(Server->srv_addr));
        if(buid_result<0){
            //print the error message
            //p- perror("bind failed. Error");
            printf("ERRO_:bind failed. Error , build result %d\n",buid_result);
            exit(buid_result);
        }else{
            //p- build done
            puts("INFO_:Build sucesse");
        }
        //start listening
        if(listen(Server->socket_desc, MAX_NUM_INCOMING_SOCKETS)==SOCKET_ERROR){
            puts("ERRO_:Socket error");
            exit(SOCKET_ERROR);

        }else{
            puts("INFO_:Server has sucessfully started to lister\n");
        }
        Server->size_sock_addr = sizeof(struct sockaddr_in);
        //p- Waiting of incomming transmissions
        printf("INFO_:Has been intialised , ready to be run\n");
        return 0;

    }else{
        puts("INFO_:server already initialised");
        return 0;
    }
}
int8_t run_server(){
    puts("INFO_:Startting server");
    int client_socket = -10;
    pid_t child_pid;
    struct sockaddr_in client;
    int c = sizeof(struct sockaddr_in);
    //init Board
    init_server();
    while(!GAME_OVER) {
        printf("INFO_: ...\n");
        client_socket= accept(Server->socket_desc, (struct sockaddr*)&client,(socklen_t*)&c);
        if(client_socket >= 0 ){
        //p- connection accepted
        printf("INFO_:connection accepted\n");
        //create pthread
        creat_fork(client_socket);
            client_socket = -10; //reset client socket
    }else
        {
            if(client_socket!=-10)printf("WARN_:accept failed with error %d\n",client_socket);
            //return -2;
        }}
    close_server();

        return 0;

    }
/*
 * Close server ,
 * to do list :
 *  -close sockets x
 *  -free connection base ~ we have sent a kill switch now need to implement
 *  -destroy board x
 */
    int8_t close_server(){
    if(GAME_OVER==0) {
        GAME_OVER = 1;
        for(int i = 0 ; i < MAX_CLIENT_NUMBER; i++){
            //kill all child threads
            if(Server->cbase[i].alive == ALIVE){
                //kill(Server->childpid[i],SIGTERM);
                destroy_connection_base(&Server->cbase[i]);
            }
        }
        puts("INFO_:Closing server");
        puts("INFO_:Closing server socket");
        close(Server->socket_desc);
        //destroy child processes
        kill(0,SIGINT);
        //destroy board
        destroy_board();
        free(Server);
        return 0;
    }
    }


