//
// Created by pookie on 03/12/17.
//

#include <stdio.h>
#include <memory.h>
#include <pthread.h>
#include "gams_status.h"
#include "reservsi_mecanics.h"
#include "board_handler.h"

//#define DEBUG
#ifdef DEBUG

#include <stdlib.h>

#endif
#define YESNO(x) (x)==1?"YES":"NO" // just enclose all of your macro in parenthesis
static game_status game;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void add_point(CLIENT_LIST player, u_int8_t points) {
    pthread_mutex_lock(&lock);
    if (player == WP)game.points_WP += points;
    else game.points_BP += points;
    pthread_mutex_unlock(&lock);
}

u_int8_t is_game_over() {
    u_int8_t retval = ((game.GAME_OVER || (game.stuck_BP && game.stuck_WP) || game.lost_WP ||
                        game.lost_BP) ? 1 : 0);
#ifdef DEBUG
    if (retval)
        printf("WARN_%d: GAME IS OVER, game over %x,stuck wp %x,stuck bp %x, lost wp %x, lost bp %x\n",
               pthread_self(),game.GAME_OVER, game.stuck_BP, game.stuck_WP, game.lost_WP, game.lost_BP);
#endif
    return retval;
}

void set_game_over() {
    pthread_mutex_lock(&lock);
    game.GAME_OVER = 1;
    pthread_mutex_unlock(&lock);
}

void set_stuck_status(CLIENT_LIST player, u_int8_t is_stuck) {
    pthread_mutex_lock(&lock);
#ifdef DEBUG
    printf("WARN_%d:Setting player to stuck %x , player %x\n",pthread_self(),is_stuck,player);
#endif
    if (player == WP)game.stuck_WP = is_stuck;
    else game.stuck_BP = is_stuck;
    pthread_mutex_unlock(&lock);
}

void init_game_status() {
    pthread_mutex_lock(&lock);
    game.GAME_OVER = 0;
    game.points_BP = 0;
    game.points_WP = 0;
    game.stuck_BP = 0;
    game.stuck_WP = 0;
    game.name_BP_length = 0;
    game.name_WP_length = 0;
    game.lost_BP = 0;
    game.lost_WP = 0;
    game.time_WP=0;
    game.time_BP=0;
    pthread_mutex_unlock(&lock);
}

void destroy_game_status() {
    pthread_mutex_lock(&lock);
    if (game.name_BP_length)free(game.name_BP);
    if (game.name_WP_length)free(game.name_WP);
    pthread_mutex_unlock(&lock);
}

void set_player_name(CLIENT_LIST player, u_int8_t lenght, u_int8_t *name) {
    pthread_mutex_lock(&lock);
    u_int8_t *nvname = (u_int8_t *) malloc(sizeof(u_int8_t) * lenght);
    memcpy(nvname, name, sizeof(char) * lenght);
    if (player == WP) {
        game.name_WP = nvname;
        game.name_WP_length = lenght;
    } else {
        game.name_BP = nvname;
        game.name_BP_length = lenght;
    }
    pthread_mutex_unlock(&lock);
}

u_int8_t get_points(CLIENT_LIST player) {
    return (player == WP) ? game.points_WP : game.points_BP;
}

u_int8_t *get_name(CLIENT_LIST player) {
    return (player == WP) ? game.name_WP : game.name_BP;

}

u_int8_t get_name_length(CLIENT_LIST player) {
    return (player == WP) ? game.name_WP_length : game.name_BP_length;
}

u_int8_t get_status_length() {
    return game.name_BP_length + game.name_WP_length + 8;
}

void set_lost_status(CLIENT_LIST player) {
    pthread_mutex_lock(&lock);
    if (player == WP)game.lost_WP = 1;
    else game.lost_BP = 1;
    pthread_mutex_unlock(&lock);
}

void msg_game_end_status() {
    puts("Game has ended stats ");
    puts("");
    puts("Player WP\n");
    printf("Name %s\nLost :%s Points :%d\nStuck :%s\nTime %lu\n",
           (char *) game.name_WP, YESNO(game.lost_WP), count_col(WHITE), YESNO(game.stuck_WP),game.time_WP);
    puts("Player BP\n");
    printf("Name %s\nLost :%s Points :%d\nStuck :%d\nTime %lu\n",
           (char *) game.name_BP, YESNO(game.lost_BP), count_col(BLACK), YESNO(game.stuck_BP),game.time_BP);
    puts("\nDo you want to player a game ?\n\n\n\n\n"
                 "Made with love by Julia Desmazes & Benjamin Levy \n\n\n");
}
void increment_time(CLIENT_LIST player,u_int8_t t){
    if(player==WP){
        game.time_WP += t;

    }else game.time_BP += t;
}