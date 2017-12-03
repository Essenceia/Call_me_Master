//
// Created by pookie on 20/11/17.
//

#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include "board_handler.h"
#include <unistd.h>

//#define DEBUG
static board *Board;

void showbits(unsigned int x) {
    int i;
    for (i = (sizeof(u_int8_t) * 8) - 1; i >= 0; i--)
        (x & (1u << i)) ? putchar('1') : putchar('0');

    printf("\n");
}

enum CELL show_at_value(u_int8_t x, u_int8_t y) {
    u_int8_t absindex, relindex, offset, buffer, val;
    absindex = gbIx(x, y);
    relindex = absindex / 4;
    val = Board->board_str[relindex];
    offset = 3 - absindex % 4;
    buffer = ((u_int8_t) val >> offset * 2);
#ifdef DEBUG
    //printf("INFO:Board geetting value coord x:%d,y:%d,value found :\n",x,y);
    //showbits((0x03&buffer));
    printf("INFO_%d:CELL at coord x:%x y:%x has value %x", getpid(), x, y, (0x03 & buffer));
#endif
    return (enum CELL) (0x03 & buffer);
}

//Get index of block according to x and y
u_int16_t gbIx(u_int8_t x, u_int8_t y) {
    return (y * Board->board_size_y + x);
}

void print_board() {

    printf("INFO_: Board view enum: \n");
    for (u_int8_t i = 0; i < Board->board_size_x; i++) {
        for (u_int8_t j = 0; j < Board->board_size_y; j++) {
            printf(".%x", (u_int8_t) show_at_value(j, i));
        }
        printf("\n");
    }

}

void init_board() {
    //set to default

    Board = (struct board *) malloc(sizeof(struct board));
    //todo find out what default last move is supposed to be
    Board->lastmove_x = DEFAULT_LAST_MOVE;
    Board->lastmove_y = DEFAULT_LAST_MOVE;
    //todo find what is the supposed size
    Board->board_size_x = DEFAULT_BOARD_SIZE_X;
    Board->board_size_y = DEFAULT_BOARD_SIZE_Y;
    //default last player
    Board->lastplayer = DEFAULT_LAST_PLAYER;
    u_int8_t sizeofboard = (Board->board_size_y * Board->board_size_x);
    Board->str_lng = Board->board_size_y * Board->board_size_x;
    /*if board size is not a multipe of 4 increment size by ones and then fill
    with undefined*/
    Board->str_lng = (Board->str_lng % 4 == 0) ? Board->str_lng / 4 : (Board->str_lng / 4) + 1;
#ifdef DEBUG
    printf("INFO_:Board size is %d ,size of string will be %d\n", (Board->board_size_y * Board->board_size_x),
           Board->str_lng);
#endif
    Board->board_str = (u_int8_t *) malloc(sizeof(u_int8_t) * Board->str_lng);
    //Board->board = (enum CELL*)malloc(sizeof(enum CELL)*sizeofboard);
    //set to empty
    //memset( Board->board, 0 ,sizeof(enum CELL)*sizeofboard );
    memset(Board->board_str, 0, sizeof(u_int8_t) * Board->str_lng);

    //fill center
    u_int8_t x = (Board->board_size_x);
    u_int8_t y = (Board->board_size_y);
    printf("INFO_:Board size x : %d, y:%d ,center x:%d, y :%d\n ", x, y, (x / 2) - 1, (y / 2) - 1);
    x = (x / 2) - 1;
    y = (y / 2) - 1;
    //toogle color
    u_int8_t col = 0;
    //set up center
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            if (col % 3 == 0)set_board(x + j, y + i, WHITE);
            else set_board(x + j, y + i, BLACK);
            col++;
        }
    }
    print_board();

}

void destroy_board() {

    //free(Board->board);
    free(Board->board_str);
    free(Board);
}

void set_board(u_int8_t x, u_int8_t y, enum CELL val) {
    u_int16_t absindex;
    u_int8_t mask, buffer, newval;
    u_int8_t offset;
    u_int8_t relindex;
    if (x < Board->board_size_x && y < Board->board_size_y) {
//#ifdef DEBUG
        printf("INFO_:Board settinf up board cell x :%x y:%x to value 0x%x\n",
               x, y, val);
//#endif
        //enum
        absindex = gbIx(x, y);
        //Board->board[absindex]= val;
        //binary
        relindex = absindex / 4;
        offset = 3 - absindex % 4;
        buffer = ((u_int8_t) val << offset * 2);
        mask = ((u_int8_t) 0b00000011 << offset * 2);
        newval = Board->board_str[relindex];
        newval = (newval & ~mask) | buffer;


        Board->board_str[relindex] = newval;
        //#ifdef DEBUG
        print_board();
//#endif
    } else {
        printf("ERRO_:Tying to wright to unused board sector x %u y %u\n", x, y);
    }
}

//todo finish format block to string
u_int8_t *format_board() {
    return Board->board_str;
}

u_int8_t get_board_string_lng() {
    return Board->str_lng;
}

/*
 * Function prepares a message for both status 1 and new move
 */
u_int8_t *board_prepare_msg() {
    u_int8_t *msg = (u_int8_t *) malloc(sizeof(u_int8_t) * (4 + Board->str_lng));
    msg[0] = Board->lastmove_x;
    msg[1] = Board->lastmove_y;
    msg[2] = Board->board_size_x;
    msg[3] = Board->board_size_y;
    memcpy(msg + 5, Board->board_str, sizeof(u_int8_t) * Board->str_lng);
    return msg;
}

/*
 * Register the new move of a player , msg will be sent
 * to controller for confirmation
 */
void register_new_player_move(u_int8_t player, u_int8_t x, u_int8_t y) {
    Board->lastmove_x = x;
    Board->lastmove_y = y;
    Board->lastplayer = player;
}

u_int8_t get_board_msg_size() {
    return Board->str_lng + 4;
}

u_int8_t get_size_x() {
    return Board->board_size_x;
}

u_int8_t get_size_y() {
    return Board->board_size_y;
}

enum CELL get_couleur_adverse(u_int8_t player) {
    enum CELL retval = NOT_USED;
    switch (player) {
        case 0x01:

            retval = WHITE;
            break;
        case 0x02:
            retval = BLACK;
            break;
        default:
            printf("ERRO_:Unknown player type %x \n", player);
            break;
    }
#ifdef DEBUG
    printf("INFO_%d,Other Player color %x\n", getpid(), retval);
#endif
    return retval;
}

enum CELL get_couleur_joueur(u_int8_t player) {
    enum CELL retval = NOT_USED;
    switch (player) {
        case 0x02:
            retval = WHITE;
            break;
        case 0x01:
            retval = BLACK;
            break;
        default:
            printf("ERRO_:Unknown player type %x \n", player);
            break;
    }
#ifdef DEBUG
    printf("INFO_%d,Our color %x\n", getpid(), retval);
#endif
    return retval;
}