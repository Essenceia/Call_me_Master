//
// Created by pookie on 12/11/17.
//

#include "lib_connection.h"
#include "connection_base_struct.h"
#include "message_helper.h"
#include<arpa/inet.h> //inet_addr
#include<string.h>    //strlen
#include <stdio.h>
#define WAIT_FOR_ALL
static u_int16_t game_step;
const enum CLIENT_LIST Play_order[2]={BP,WP};

void warn_late_controler(void){
    printf("WARN_:Hey controler is running late \n");
}
void warn_late_player(void){
    printf("WARN_:Hey player is running late \n");
}
client_warining get_player_waring(){
    return &warn_late_player;
}
client_warining get_gcontroler_waring(){
    return &warn_late_controler;
}
/*
 * todo - implement
 * Connection handler for player
 * On passe une connection base au client
 */
void player_handler(void* player, int socket){
    printf("INFO_%d:Starting client handler\n",getpid());
    u_int8_t max_wait_time=0;
    struct connection_base *Player = (struct connection_base*)player;
    //send ok message
    u_int8_t *send_msg = msg_ok(Player->client_type);

    ssize_t sendres= send(socket,send_msg,5,0);
    free(send_msg);
    printf("INFO_%d: Message OK send to client with result %u \n",getpid(),sendres);
    //check everbody is connected
#ifdef WAIT_FOR_ALL // wait for all clients to be connected
    while (!has_all_clients()&& max_wait_time<DEFAULT_TIME_DIE){
    sleep(1);
        max_wait_time+=1;
        printf("INFO_%d:Waiting for other clients to connect\n",getpid());
    }

    //game
    while (Player->alive==ALIVE && (!GAME_OVER) && has_all_clients()){
#else
        while (Player->alive==ALIVE && (!GAME_OVER) ){
#endif
        //check if our turn
        printf("INFO_%d:In while loop \n",getpid());
        if(Play_order[(game_step%2)]==Player->client_type){
            //send New move
            printf("INFO_%d:Out turn to play\n",getpid());

            //start timeout counter

            //wait for move

            //check move
        }
    }
    unregister_client(Player->client_type);
    //autodestruction
    destroy_connection_base(Player);
}
/*
 * todo - implement
 * Connection handler for Game Controller
 *
 */
void gclient_handler(void *controler,int socket){
    //set game counter to zero
    game_step = 0;

}
//todo depreciate
void send_init_message(int socket){

    write(socket,INIT_MESSAGE,strlen(INIT_MESSAGE));
}
//todo depricate
void handler_message(char *recived_message, int msg_leng, int socket){
    //handle the message treatement
#ifdef DEBUG
    puts("Message is beeing treated");
    write(socket,recived_message,msg_leng);
#endif
}
//todo depricate
void *handler_connection(void* dest_socket){
    int socket = *(int*)dest_socket;
    int size_read;
    char client_message[MAX_CLIENT_MESSAGE_SIZE];
    send_init_message(socket);
    while( (size_read = recv(socket,client_message,MAX_CLIENT_MESSAGE_SIZE,0))>0 ){
        handler_message(client_message, strlen(client_message), socket);
    }
    if(size_read == 0){
        //p- transmission finished
        fflush(stdout);
    }else if(size_read == -1){
        //p- recv failed
    }
    free(dest_socket);
    close(socket);
    return 0;

}
struct comm_message* recive_msg(int socket){
    u_int8_t *recv_msg;
    recv_msg = (u_int8_t *)malloc(sizeof(u_int8_t)*MAX_CLIENT_MESSAGE_SIZE);
    struct comm_message* recvcom = NULL;
    ssize_t size_msg = recv(socket,recv_msg,MAX_CLIENT_MESSAGE_SIZE,0);
    if(size_msg > 0){
        recvcom = parse_recv_msg(recv_msg,size_msg);
#ifdef DEBUG
        if(recv != NULL){
            printf("INFO_%d:Message was recived form client\n",getpid());
        }else{
            printf("WARN_%d:Error in recived message\n",getpid());
        }
#endif
    }
    #ifdef DEBUG
    else{

        puts("INFO_:Noting to read form clients");
    }
    #endif
    free(recv_msg);
    return recvcom;
}