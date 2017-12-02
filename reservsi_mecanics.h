//
// Created by pookie on 02/12/17.
//

#ifndef CPROJ_RESERVSI_MECANICS_H
#define CPROJ_RESERVSI_MECANICS_H

#include "message_parser.h
void init_game();
struct comm_message* check_player_move(u_int8_t player,u_int8_t x,u_int8_t y);
struct comm_message* new_move_for_player(u_int8_t player);
void end_game();

#endif //CPROJ_RESERVSI_MECANICS_H
