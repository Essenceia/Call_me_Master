//
// Created by pookie on 20/11/17.
//

#ifndef CPROJ_BOARD_HANDLER_H
#define CPROJ_BOARD_HANDLER_H

#define DEFAULT_BOARD_SIZE_X 8
#define DEFAULT_BOARD_SIZE_Y 8
#define DEFAULT_LAST_MOVE 0xff
#define DEFAULT_LAST_PLAYER 1
#define BOARD_MSG_ADDITIONL_DATA 4

#include <stdlib.h>
enum CELL{EMPTY = 0, WHITE = 1,BLACK = 2,NOT_USED=3  };
enum CELL_PROX{ PROX_NONE = 0,PROX_WHITE =1,PROX_BLACK=2,PROX_BOTH=3,PROX_USED =4};
typedef struct{
    u_int8_t lastmove_x;
    u_int8_t lastmove_y;
    u_int8_t lastplayer;
    u_int8_t board_size_x;
    u_int8_t board_size_y;
    //enum CELL * board;
    u_int8_t* board_str;
    u_int8_t str_lng;

} board;
typedef struct{
    enum CELL_PROX **cboard;//check board
}move_board;
void set_board(u_int8_t x,u_int8_t y , enum CELL val);
void destroy_board();
void init_board();
void init_move_board();
u_int8_t has_prox_col(enum CELL prox_col,u_int8_t x,u_int8_t y);//check if we have a certain color in proximity
void update_prox_col(enum CELL prox_col,u_int8_t x,u_int8_t y);//update status of proximity bits when we set a new cell value
enum CELL show_at_value(u_int8_t x, u_int8_t y);
enum CELL get_couleur_joueur(u_int8_t player);

enum CELL get_couleur_adverse(u_int8_t player);
u_int8_t get_size_x();
u_int8_t get_size_y();
u_int16_t gbIx(u_int8_t x,u_int8_t y);
u_int8_t get_board_string_lng();
u_int8_t* board_prepare_msg();
u_int8_t get_board_msg_size();
void register_new_player_move(u_int8_t player,u_int8_t x,u_int8_t y);
#endif //CPROJ_BOARD_HANDLER_H
