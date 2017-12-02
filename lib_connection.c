//
// Created by pookie on 12/11/17.
//

#include "lib_connection.h"
#include "connection_base_struct.h"
#include "message_helper.h"
#include "message_defines.h"
#include<arpa/inet.h> //inet_addr
#include<string.h>    //strlen
#include <stdio.h>
#include <unistd.h>

#define WAIT_FOR_ALL
typedef struct TUBE{
    //u_int8_t *tobesent;
    //size_t msglng;
    struct comm_message *tosend;
    u_int8_t active;
};
static struct TUBE message_share[3];
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
void player_handler(void* player){
    printf("INFO_%d:Starting client handler\n",getpid());
    u_int8_t max_wait_time=0;
    struct connection_base *Player = (struct connection_base*)player;
    //send ok message
    u_int8_t *buf,msgsize;
    msg_player_ok(Player->client_type, Player->dest_socket);
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
            //send turn message
           next_turn(Player->dest_socket);

            //todo start timeout counter

            //wait for move recived
            Player->rev_msg = recive_msg(Player->dest_socket);
            if(Player->rev_msg!= NULL){
                //we just recived a message from the client with the new turn
                if(Player->rev_msg->type == NEW_MOVE) {
                    //todo stop timer
                    //check move - send to controller
                    message_share[GC].tosend =recapsulate_for_controller(Player->rev_msg,Player->client_type);
                    message_share[GC].active = 1;


                    //wait untill we get a responsse from the game clontroller
                    while (!(message_share[Player->client_type].active)) {
                        sleep(1);
                        printf("INFO_%d:Player waiting for client response");
                    }
                    //have recived message from client - send allong
                    message_share[Player->client_type].active=0;
                    Player->tosend_msg = message_share[Player->client_type].tosend;
                    switch (Player->tosend_msg->type){
                        case OK_NOK:
                            if(Player->tosend_msg->msg[0]==0){
                                GAME_OVER = 1;
                                Player->win=LOSER;
                            }
                            //increment play step and give hand to next player
                            break;
                        case SKIP_TURN:
                            //todo tel player to skip it's turn
                            printf("INFO_%d:Player %x will be skipping his turn on next round\n",getpid(),
                            Player->client_type);
                            break;
                    }
                    //send allong
                    if(sendof(Player->dest_socket,Player->tosend_msg)<0){
                        Player->win=LOSER;
                        Player->alive =DEAD;
                        GAME_OVER=1;
                        printf("ERRO_%d:Error sending packet player %x eliminates\n",getpid(),
                        Player->client_type);
                    }

                }


            }
            game_step++;

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
void gclient_handler(void *controler){
    //declare ressources
    struct comm_message *send_msg;
    struct connection_base *Contro = (struct connection_base*)controler;
    //init tube as unactive
    message_share[0].active = 0;
    message_share[1].active = 0;
    message_share[2].active = 0;
    enum CLIENT_LIST player;
    //set game counter to zero
    game_step = 0;
    while(!GAME_OVER){
        if(message_share[GC].active){
            Contro->tosend_msg=message_share[GC].tosend;
         //we want to share data to the client
            if(sendof(Contro->dest_socket,Contro->tosend_msg)==0){
                printf("INFO_%d:Message sucessfully sent to client \n",getpid());
            }else printf("ERRO_%d:Message failed sent to client \n",getpid());
            message_share[GC].active = 0;

        }
        //check if we recived new msg
        Contro->rev_msg = recive_msg(Contro->dest_socket);
        if(Contro->rev_msg!= NULL){
         //todo - check for recived messages
            if(Contro->rev_msg->type==SKIP_TURN || Contro->rev_msg->type == OK_NOK){
                //forward send message to player
                player = (enum CLIENT_LIST)Contro->rev_msg->msg[0];
                message_share[player].tosend = recapsulate_for_player(Contro->rev_msg);
                message_share[player].active = 1;
                printf("INFO_Client: Forwarding message to player handler with id %x msg : %s \n",
                player,(char*)message_share[player].tosend->msg);


            }else{
                printf("ERRO_%d: Unexpected message recived form controller message is of type %x",getpid(),
                Contro->rev_msg->type);
            }destroy_msg(Contro->rev_msg);

        }
    }

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