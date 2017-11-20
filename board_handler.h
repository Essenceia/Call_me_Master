//
// Created by pookie on 20/11/17.
//

#ifndef CPROJ_BOARD_HANDLER_H
#define CPROJ_BOARD_HANDLER_H

#include <zconf.h>
#define DEFAULT_BOARD_SIZE_X 8
#define DEFAULT_BOARD_SIZE_Y 8
typedef struct board{
    u_int8_t lastmove_x;
    u_int8_t lastmove_y;
    u_int8_t board_size_x;
    u_int8_t board_size_y;
    u_int8_t* board;

};
typedef enum CELL{EMPTY = 0, WHITE = 1,BLACK = 2,NOT_USED=3  };
void set_board(u_int8_t x,u_int8_t y , enum CELL val);
void destroy_board();
u_int8_t get_board_status(u_int8_t x, u_int8_t y);
#endif //CPROJ_BOARD_HANDLER_H
