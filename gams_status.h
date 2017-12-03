//
// Created by pookie on 03/12/17.
//

#ifndef CPROJ_GAMS_STATUS_H
#define CPROJ_GAMS_STATUS_H

#include <stdlib.h>
#include "client_registration.h"

typedef struct{
    u_int8_t points_BP;
    u_int8_t points_WP;
    u_int8_t GAME_OVER;
    u_int8_t stuck_BP;
    u_int8_t stuck_WP;
    u_int8_t name_BP_length;
    u_int8_t name_WP_length;
    u_int8_t * name_WP;
    u_int8_t * name_BP;
}game_status;
//set game status
void add_point(enum CLIENT_LIST player,u_int8_t points);
u_int8_t is_game_over();
void set_game_over();
void set_stuck_status(enum CLIENT_LIST player,u_int8_t is_stuck);
void init_game_status();
void destroy_game_status();
void set_player_name(enum CLIENT_LIST player,u_int8_t lenght,u_int8_t * name);
u_int8_t get_points(enum CLIENT_LIST player);
u_int8_t *get_name(enum CLIENT_LIST player);
u_int8_t get_name_length(enum CLIENT_LIST player);
u_int8_t get_status_length();
#endif //CPROJ_GAMS_STATUS_H
