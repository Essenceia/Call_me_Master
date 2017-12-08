//
// Created by pookie on 02/12/17.
//

#ifndef CPROJ_RESERVSI_MECANICS_H
#define CPROJ_RESERVSI_MECANICS_H

#include "message_parser.h"
#include "client_registration.h"

void init_game();
u_int8_t check_player_move(int socket,CLIENT_LIST player,comm_message* player_sent_move);
u_int8_t new_move_for_player(int socket,CLIENT_LIST player);
u_int8_t send_status(int socket);
void end_game();
u_int8_t is_game_over();
void send_game_end(int socket);
CLIENT_LIST get_turn();
u_int8_t count_col(u_int8_t col);

#endif //CPROJ_RESERVSI_MECANICS_H
