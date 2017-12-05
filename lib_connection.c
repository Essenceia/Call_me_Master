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
#include "reservsi_mecanics.h"
#include "gams_status.h"

//#define WAIT_FOR_ALL
static u_int16_t game_step;
const CLIENT_LIST Play_order[2] = {BP, WP};


/*
 * Connection handler for player
 * On passe une connection base au client
 */
void player_handler(void *player) {
    printf("INFO_%d:Starting client handler\n", getpid());
    connection_base *Player = (connection_base *) player;
    Player->clock=init_TimeKeeper(30);
    //set timeout on socket opperations
    struct timeval tv = {1, 0};
    setsockopt(Player->dest_socket, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv, sizeof(struct timeval));

    //send ok message
    msg_player_ok(Player->client_type, Player->dest_socket);
    //check everbody is connected
#ifdef WAIT_FOR_ALL // wait for all clients to be connected
    while (!has_all_clients() && max_wait_time < DEFAULT_TIME_DIE) {
        sleep(2);
        max_wait_time += 1;
        //printf("INFO_%d:Waiting for other clients to connect\n", getpid());
    }

    //game
    while (Player->alive == ALIVE && (!GAME_OVER) && has_all_clients())
#else
        while (Player->alive==ALIVE && (!is_game_over()) )
#endif
        {
        //check if our turn

        printf("INFO_%d:In while loop , player type %x , game_step %x \n", getpid(),Player->client_type,game_step%2);
        if (Play_order[(game_step % 2)] == Player->client_type) {
            //send New move
            printf("INFO_%d:Out turn to play\n", getpid());
            //send turn message - check if should ship next turn
            if (new_move_for_player(Player->dest_socket, Player->client_type)) {

                timer_start(Player->clock);


                //wait for move recived
                while(timer_check_elapsed_time(Player->clock)==TIMER_COUNTING) {
                    sleep(1);
                    Player->rev_msg = recive_msg(Player->dest_socket);
                    if (Player->rev_msg != NULL) {
                        timer_stop(Player->clock);
                        if (Player->rev_msg->type == NEW_MOVE) {

                            switch (check_player_move(Player->dest_socket, Player->client_type, Player->rev_msg)) {
                                case 0x01:
                                    printf("INFO_%d:Move recived from player %x is valide\n", getpid(),
                                           Player->client_type);
                                    break;
                                case 0x00:
                                    printf("ERRO_%d:Move recived from player %s is NOT valide\n", getpid(),
                                           Player->client_type);
                                    set_lost_status(Player->client_type);
                                    set_game_over();
                                    break;
                                default:
                                    printf("ERRO_%d:Lookes like something went very wring with move check, ending game\n",
                                           getpid());
                                    set_game_over();
                                    break;
                            }
                        }
                    }
                    destroy_msg(Player->rev_msg);
                }if(timer_check_elapsed_time(Player->clock)==TIMER_OVERFLOW){
                    set_lost_status(Player->client_type);
                    Player->alive=DEAD;
                }
            }
            game_step++;
        }else{
            Player->rev_msg = recive_msg(Player->dest_socket);
            if(Player->rev_msg!=NULL) {
                switch (Player->rev_msg->type) {
                    case PING:
                        msg_ping(Player->dest_socket);
                        break;
                    default:
                        printf("ERRO_%d:Recived unexpected message from controller, type %d\n"
                                       "         It was not players turn, you lose\n",
                               getpid(), Player->rev_msg->type);


                        break;
                }
                destroy_msg(Player->rev_msg);
            }
        }

    }
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
    game_step = 0;
    u_int16_t previous_step = game_step;
    struct timeval tv = {1, 0};
    setsockopt(Contro->dest_socket, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv, sizeof(struct timeval));

    while ((!is_game_over()) && has_all_clients()){
        sleep(1);
       if(previous_step!=game_step){
           //send statup1 to controller
           send_status(Contro->dest_socket);
       }
        Contro->rev_msg = recive_msg(Contro->dest_socket);
        if(Contro->rev_msg!=NULL) {
            switch (Contro->rev_msg->type) {
                case PING:
                    msg_ping(Contro->dest_socket);
                    break;
                default:printf("ERRO_%d:Recived unexpected message from controller, type %d",
                    getpid(),Contro->rev_msg->type);
                    break;
            }
            destroy_msg(Contro->rev_msg);
        }
    }
    destroy_connection_base(Contro);


}
comm_message *recive_msg(int socket) {
    u_int8_t *recv_msg;
    recv_msg = (u_int8_t *) malloc(sizeof(u_int8_t) * MAX_CLIENT_MESSAGE_SIZE);
    comm_message *recvcom = NULL;

    ssize_t size_msg = recv(socket, recv_msg, MAX_CLIENT_MESSAGE_SIZE, 0);
    if (size_msg > 0) {
        recvcom = parse_recv_msg(recv_msg, size_msg);
#ifdef DEBUG
        if (recvcom != NULL) {
            printf("INFO_%d:Message was recived form client\n", getpid());

        } else {
            printf("WARN_%d:Error in recived message\n", getpid());
        }
#endif
    }
#ifdef DEBUG
    else {

        puts("INFO_:Noting to read form clients");
    }
#endif
    free(recv_msg);
    return recvcom;
}