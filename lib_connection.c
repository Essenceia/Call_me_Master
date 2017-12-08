//
// Created by pookie on 12/11/17.
//

#include "lib_connection.h"
#include "connection_base_struct.h"
#include "message_helper.h"
#include "message_defines.h"
#include<arpa/inet.h> //inet_addr
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "reservsi_mecanics.h"
#include "gams_status.h"
#include "game_step.h"
#include "message_parser.h"
#include "print_in_color.h"
//#define WAIT_FOR_ALL
//#define DEBUG

/*
 * Connection handler for player
 * On passe une connection base au client
 */
const CLIENT_LIST Play_order[2] = {BP, WP};

void player_handler(void *player) {
    set_process_color(pthread_self());
    printf("INFO_%d:Starting client handler\n", pthread_self());
    connection_base *Player = (connection_base *) player;

    //set timeout on socket opperations
    struct timeval tv = {1, 0};
    setsockopt(Player->dest_socket, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv, sizeof(struct timeval));

    //send ok message
    msg_player_ok(Player->client_type, Player->dest_socket);

    destroy_msg(Player->rev_msg);
    //check everbody is connected
#ifdef WAIT_FOR_ALL // wait for all clients to be connected
    while (!has_all_clients() && max_wait_time < DEFAULT_TIME_DIE) {
        sleep(2);
        max_wait_time += 1;
        //printf("INFO_%d:Waiting for other clients to connect\n", pthread_self());
    }

    //game
    while (Player->alive == ALIVE && (!GAME_OVER) && has_all_clients())
#else
        while (Player->alive==ALIVE && (is_game_over()==0) )
#endif
        {
        //check if our turn

#ifdef DEBUG
        printf("INFO_%d:In while loop , player type %x ,turn of type %x game_step %x \n", pthread_self(),Player->client_type,Play_order[(get_game_step() % 2)],get_game_step());
#endif
        if (Play_order[(get_game_step() % 2)] == Player->client_type) {
            //send New move
#ifdef DEBUG
            printf("INFO_%d:Our turn get to play\n", pthread_self());
#endif
            //send turn message - check if should ship next turn
            if (new_move_for_player(Player->dest_socket, Player->client_type)) {

                timer_start(Player->client_type);


                //wait for move recived
                while(timer_check_elapsed_time(Player->client_type)==TIMER_COUNTING) {
                    Player->rev_msg = recive_msg(Player->dest_socket);
                    if (Player->rev_msg != NULL) {
                        increment_time(Player->client_type,get_elapsed(Player->client_type));
                        timer_stop(Player->client_type);
                        if (Player->rev_msg->type == NEW_MOVE) {
                            switch (check_player_move(Player->dest_socket, Player->client_type, Player->rev_msg)) {
                                case 0x01:
                                    set_process_color(pthread_self());
                                    printf("INFO_%d:Move recived from player %x is valide\n", pthread_self(),
                                           Player->client_type);
                                    break;
                                case 0x00:
                                    set_process_color(pthread_self());
                                    printf("ERRO_:Move recived from player is NOT valide\n");
                                    set_lost_status(Player->client_type);
                                    set_game_over();
                                    break;
                                default:
                                    set_process_color(pthread_self());
                                    printf("ERRO_%d:Lookes like something went very wring with move check, ending game\n",
                                           pthread_self());
                                    set_game_over();
                                    break;
                            }
                        }else{
                            set_process_color(pthread_self());
                            printf("WARN_%d:Was expecting new move but got type %x\n",pthread_self(),Player->rev_msg->type);
                        }
                        destroy_msg(Player->rev_msg);
                    }

                }if(timer_check_elapsed_time(Player->client_type)==TIMER_OVERFLOW){
                    set_lost_status(Player->client_type);
                    Player->alive=DEAD;
                }
            }
           incr_game_step();
        }else{
            Player->rev_msg = recive_msg(Player->dest_socket);
            if(Player->rev_msg!=NULL) {
                switch (Player->rev_msg->type) {
                    case PING:
                        msg_ping(Player->dest_socket);
                        break;
                    default:
                        set_process_color(pthread_self());
                        printf("ERRO_%d:Recived unexpected message from controller, type %d\n"
                                       "         It was not players turn, you lose\n",
                               pthread_self(), Player->rev_msg->type);


                        break;
                }
                destroy_msg(Player->rev_msg);
            }
        }

    }
#ifdef DEBUG
    printf("INFO_%d: Sending end of game message alive? %x end of game %x\n",Player->alive==ALIVE,
    is_game_over()==0);
#endif
    send_game_end(Player->dest_socket);
    msg_game_end_status();
    unregister_client(Player->client_type);
    destroy_connection_base(Player);
}
/*
 * Connection handler for Game Controller
 *
 */
void gclient_handler(void *controler) {
    //declare ressources
    connection_base *Contro = (connection_base *) controler;
    //set game counter to zero
    u_int16_t previous_step = get_game_step();
    struct timeval tv = {1, 0};
    setsockopt(Contro->dest_socket, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv, sizeof(struct timeval));

    while ((!is_game_over()) && has_all_clients()){
       // sleep(1);
       if(previous_step!=get_game_step()){
           //send statup1 to controller
           send_status(Contro->dest_socket);
       }
        Contro->rev_msg = recive_msg(Contro->dest_socket);
        if(Contro->rev_msg!=NULL) {
            switch (Contro->rev_msg->type) {
                case PING:
                    msg_ping(Contro->dest_socket);
                    break;
                default:
                    set_process_color(pthread_self());
                    printf("ERRO_%d:Recived unexpected message from controller, type %d",
                    pthread_self(),Contro->rev_msg->type);
                    break;
            }
            destroy_msg(Contro->rev_msg);
        }
    }
    destroy_connection_base(Contro);


}
comm_message *recive_msg(int socket) {
    u_int8_t *recv_msg = NULL;
    recv_msg = (u_int8_t *) malloc(sizeof(u_int8_t) * MAX_CLIENT_MESSAGE_SIZE);
    comm_message *recvcom = NULL;

    ssize_t size_msg = recv(socket, recv_msg, MAX_CLIENT_MESSAGE_SIZE, 0);
    if (size_msg > 0) {
        recvcom = parse_recv_msg(recv_msg, size_msg);
#ifdef DEBUG
        if (recvcom != NULL) {
            printf("INFO_%d:Message was recived form client of type %x \n", pthread_self(),recvcom->type);

        } else {
            printf("WARN_%d:Error in recived message\n", pthread_self());
        }
#endif
    }
#ifdef DEBUG
    else {

        puts("INFO_:Noting to read form clients");
    }
#endif
    if(recv_msg!=NULL)free(recv_msg);
    return recvcom;
}