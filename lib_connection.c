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
#include "reservsi_mecanics.h"
#include "client_registration.h"
#include "gams_status.h"
//#define WAIT_FOR_ALL
typedef struct TUBE {
    //u_int8_t *tobesent;
    //size_t msglng;
    comm_message *tosend;
    u_int8_t active;
};
static struct TUBE message_share[3];
static u_int16_t game_step;
const enum CLIENT_LIST Play_order[2] = {BP, WP};

void warn_late_controler(void) {
    printf("WARN_:Hey controler is running late \n");
}

void warn_late_player(void) {
    printf("WARN_:Hey player is running late \n");
}

client_warining get_player_waring() {
    return &warn_late_player;
}

client_warining get_gcontroler_waring() {
    return &warn_late_controler;
}

/*
 * todo - implement
 * Connection handler for player
 * On passe une connection base au client
 */
void player_handler(void *player) {
    printf("INFO_%d:Starting client handler\n", getpid());
    u_int8_t max_wait_time = 0;
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
            sleep(1);
        printf("INFO_%d:In while loop , player type %x , game_step %x \n", getpid(),Player->client_type,game_step%2);
        if (Play_order[(game_step % 2)] == Player->client_type) {
            //send New move
            printf("INFO_%d:Out turn to play\n", getpid());
            //send turn message - check if should ship next turn
            if (new_move_for_player(Player->dest_socket, Player->client_type)) {

                //todo start timeout counter
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
                                    printf("INFO_%d:Move recived from player %s is valide\n", getpid(),
                                           Player->client_name);
                                    break;
                                case 0x00:
                                    printf("ERRO_%d:Move recived from player %s is NOT valide\n", getpid(),
                                           Player->client_name);
                                    Player->win = LOSER;
                                    Player->alive = DEAD;
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
                }if(timer_check_elapsed_time(Player->clock)==TIMER_OVERFLOW){
                    Player->win=LOSER;
                    Player->alive=DEAD;
                }
            }
            game_step++;
        }

    }
    send_game_end(Player->dest_socket);
    if(Player->win==LOSER)printf("\nLOSES player %s on id %x , tread %d\n",Player->client_name,Player->client_type,getpid());
    else printf("\nWIN player %s on id %x , tread %d\n",Player->client_name,Player->client_type,getpid());
    unregister_client(Player->client_type);
    destroy_connection_base(Player);
}
/*
 * todo - implement
 * Connection handler for Game Controller
 *
 */
void gclient_handler(void *controler) {
    //declare ressources
    connection_base *Contro = (connection_base *) controler;
    //set game counter to zero
    game_step = 0;
    u_int16_t previous_step = game_step;
   while ((!is_game_over()) && has_all_clients()){
        sleep(2);
       if(previous_step!=game_step){
           //send statup1 to controller
           send_status(Contro->dest_socket);
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
        if (recv != NULL) {
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