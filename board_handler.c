//
// Created by pookie on 20/11/17.
//

#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include "board_handler.h"
#include <pthread.h>

#define SHOW_MODIF
//#define DEBUG
#ifdef DEBUG
//#define DEBUG_SUPER
#endif
//#define DEBUG_SPECIAL
static board *Board;
static move_board *CBoard;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void showbits(unsigned int x) {
#ifdef DEBUG_SPECIAL
    int i;
    for (i = (sizeof(u_int8_t) * 8) - 1; i >= 0; i--)
        (x & (1u << i)) ? putchar('1') : putchar('0');

    printf("\n");
#endif
}

CELL show_at_value(u_int8_t x, u_int8_t y) {
    u_int8_t absindex, relindex, offset, buffer, val;
    absindex = gbIx(x, y);
    relindex = absindex / 4;
    val = Board->board_str[relindex];
    offset = 3 - absindex % 4;
    buffer = ((u_int8_t) val >> offset * 2);
#ifdef DEBUG_SUPER
    //printf("INFO:Board geetting value coord x:%d,y:%d,value found :\n",x,y);
    showbits((0x03&buffer));
    //printf("INFO_%d:CELL at coord x:%x y:%x has value %x\n", pthread_self(), x, y, (0x03 & buffer));
#endif
    return (CELL) (0x03 & buffer);
}

//Get index of block according to x and y
u_int16_t gbIx(u_int8_t x, u_int8_t y) {
    return (y * Board->board_size_y + x);
}

void print_board() {
    printf("\x1B[0m");
    printf("INFO_: Board view : \n");
    for (u_int8_t i = 0; i < Board->board_size_x; i++) {
        for (u_int8_t j = 0; j < Board->board_size_y; j++) {
            printf(".%x", (u_int8_t) show_at_value(j, i));
        }
        printf("\n");
    }
    printf("INFO_: Neihbour board view: \n");
    for (u_int8_t i = 0; i < Board->board_size_x; i++) {
        for (u_int8_t j = 0; j < Board->board_size_y; j++) {
            printf(".%x", (u_int8_t) CBoard->cboard[i][j]);
        }
        printf("\n");
    }

}

void init_board() {
    //set to default

    Board = (board *) malloc(sizeof(board));
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
    //Board->board = (CELL*)malloc(sizeof(CELL)*sizeofboard);
    //set to empty
    //memset( Board->board, 0 ,sizeof(CELL)*sizeofboard );
    memset(Board->board_str, 0, sizeof(u_int8_t) * Board->str_lng);
    init_move_board();

    //fill center
    u_int8_t x = (Board->board_size_x);
    u_int8_t y = (Board->board_size_y);
#ifdef DEBUG
    printf("INFO_:Board size x : %d, y:%d ,center x:%d, y :%d\n ", x, y, (x / 2) - 1, (y / 2) - 1);
#endif
    x = (x / 2) - 1;
    y = (y / 2) - 1;
    //toogle color
    u_int8_t col = 0;
    //set up center
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            if (col % 3 == 0) {
                set_board(x + j, y + i, WHITE);
                update_prox_col(WHITE, x + j, y + i);
            } else {
                set_board(x + j, y + i, BLACK);
                update_prox_col(BLACK, x + j, y + i);
            }
            col++;
        }
    }
#ifdef DEBUG
    printf("WARN_:Initialising board \n");
#endif
#ifdef SHOW_MODIF
    print_board();
#endif
}

void destroy_board() {
    pthread_mutex_lock(&lock);
    //free(Board->board);
    if (Board != NULL) {
        free(Board->board_str);
        free(Board);
        Board = NULL;
    }
    pthread_mutex_unlock(&lock);
}

void set_board(u_int8_t x, u_int8_t y, CELL val) {
    pthread_mutex_lock(&lock);
    u_int16_t absindex;
    u_int8_t mask, buffer, newval;
    u_int8_t offset;
    u_int8_t relindex;
#if defined(SHOW_MODIF) && defined(DEBUG)
    printf("INFO_:Board settinf up board cell x :%x y:%x to value 0x%x\n",
           x, y, val);
#endif
    if (x < Board->board_size_x && y < Board->board_size_y) {

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
        update_prox_col(val, x, y);//experiment // nope has to be done at the end of a valide move
#ifdef SHOW_MODIF
        printf("INFO_:Have just updated board\n");
        print_board();
#endif
    } else {
        printf("ERRO_:Tying to wright to unused board sector x %u y %u\n", x, y);
    }
    pthread_mutex_unlock(&lock);
}

u_int8_t *format_board() {
    return Board->board_str;
}

u_int8_t get_board_string_lng() {
#ifdef DEBUG_SPECIAL
    printf("INFO_:Board length is %x ",Board->str_lng
    );
#endif
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
    memcpy(msg + 4, Board->board_str, Board->str_lng);
#ifdef DEBUG_SPECIAL
    printf("INFO_:Spedial print de foux :\n");
    for(int i = 0 ; i < Board->str_lng ; i++){
        showbits(Board->board_str[i]);
    }
    printf("INFO_:Board message :\n");
    for(int i = 0 ; i < 4 + Board->str_lng ;i ++){
        printf("_%x",msg[i]);
    }
#endif
    return msg;
}

/*
 * Register the new move of a player , msg will be sent
 * to controller for confirmation
 */
void register_new_player_move(u_int8_t player, u_int8_t x, u_int8_t y) {
    pthread_mutex_lock(&lock);
    Board->lastmove_x = x;
    Board->lastmove_y = y;
    Board->lastplayer = player;
    update_prox_col(get_couleur_joueur(player), x, y);
    pthread_mutex_unlock(&lock);
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

CELL get_couleur_adverse(u_int8_t player) {
    CELL retval = NOT_USED;
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
#ifdef DEBUG_SUPER
    printf("INFO_%d,Other Player color %x\n", pthread_self(), retval);
#endif
    return retval;
}

CELL get_couleur_joueur(u_int8_t player) {
    CELL retval = NOT_USED;
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
#ifdef DEBUG_SUPER
    printf("INFO_%d,Our color %x\n", pthread_self(), retval);
#endif
    return retval;
}

void init_move_board() {
    CBoard = (move_board *) malloc(sizeof(move_board));
    CBoard->cboard = (CELL_PROX **) malloc(sizeof(CELL_PROX *) * Board->board_size_x);
    for (u_int8_t i = 0; i < Board->board_size_x; i++) {
        CBoard->cboard[i] = (CELL_PROX *) malloc(sizeof(CELL_PROX) * Board->board_size_y);
        //memset(CBoard->cboard[i], 0, sizeof(CELL_PROX) * Board->board_size_y);
    }
#ifdef DEBUG_SUPER
    printf("INFO_:Filling board\n");
#endif
    for (u_int8_t i = 0; i < Board->board_size_x; i++) {
        for (u_int8_t j = 0; j < Board->board_size_y; j++) {
            CBoard->cboard[i][j] = PROX_NONE;
        }
    }

#ifdef DEBUG
    for (u_int8_t i = 0; i < Board->board_size_x; i++) {
        for (u_int8_t j = 0; j < Board->board_size_y; j++) {
            printf(".%x",CBoard->cboard[i][j]);
        }
        printf("\n");
    }
#endif
}

u_int8_t has_prox_col(CELL prox_col, u_int8_t x, u_int8_t y) {//check if we have a certain color in proximity
    CELL_PROX clst = CBoard->cboard[x][y];
    return ((prox_col == WHITE && (clst == PROX_WHITE || clst == PROX_BOTH))
            || (prox_col == BLACK && (clst == PROX_BLACK || clst == PROX_BOTH)))
           ? 1 : 0;
}

u_int8_t count_prox_col(CELL nv_col, int8_t sx, int8_t sy) {

    u_int8_t retval = 0;
#ifdef DEBUG_SUPER
    printf("INFO_:Inspecting Count Proc Col cell x:%x y:%x looking if is of color %x\n",sx,sy,nv_col);
#endif
    if (sx >= 0 && sx < Board->board_size_x &&
        sy >= 0 && sy < Board->board_size_y) {
        //check previous value of cell
#ifdef DEBUG_SUPER
        printf("INFO_:Inspecting cell x:%x y:%x looking if is of color %x\n",sx,sy,nv_col);
#endif
        if (nv_col == show_at_value((u_int8_t) sx, (u_int8_t) sy)) {
            return 1;
        }

    }
    return retval;
}

u_int8_t count_round_prox_col(CELL nv_col, int8_t sx, int8_t sy) {
    u_int8_t retval = 0;
    for (int8_t dx = -1; dx < 2; dx++) {
        for (int8_t dy = -1; dy < 2; dy++) {
            if (!(dx == 0 && dy == 0)) {
                retval += count_prox_col(nv_col, sx + dx, sy + dy);
                if (retval) {
#ifdef DEBUG_SUPER
                    printf("INFO_:Neihbout found of col %x to empty cell at pos x:%x y:%x\n",nv_col,sx,sy);
#endif
                    goto found;
                }//so we can stop after just having found one valide value
            }
        }
    }
    found:
    return retval;
}

void
update_prox_col(CELL prox_col, u_int8_t x, u_int8_t y)//update status of proximity bits when we set a new cell value
{
    //no need to lock already protected by calling function
#ifdef DEBUG_SPECIAL
    printf("INFO_:Stating neighbours update of cell in x:%x y:%x\n",x,y );
#endif
    /*for(u_int8_t ax = 0 ; ax < get_size_x(); ax++) {
        for (u_int8_t ay = 0; ay < get_size_y(); ay++) {*/
    //we have changed color of cell in x,y
    int8_t ix, iy;
    u_int8_t pb = 0, pw = 0;
    //start by setting the pos cell to used
    CBoard->cboard[x][y] = PROX_USED;
    //look at all neihbours that are empty
    for (int8_t dx = -1; dx < 2; dx++) {
        ix = (int8_t) x + dx;
        for (int8_t dy = -1; dy < 2; dy++) {
            iy = (int8_t) y + dy;
            if (ix >= 0 && ix < Board->board_size_x &&
                iy >= 0 && iy < Board->board_size_y) {
                if (CBoard->cboard[ix][iy] != PROX_USED) {
#ifdef DEBUG_SUPER
                    printf("INFO_:Looking at neighbours of cell in x:%x y:%x\n",ix,iy );
#endif
                    pw = count_round_prox_col(WHITE, ix, iy);
                    pb = count_round_prox_col(BLACK, ix, iy);
                    if (pw || pb) {
                        //we have at least one neinhour
                        if (pw && pb)CBoard->cboard[ix][iy] = PROX_BOTH;
                        else {
                            if (pw)CBoard->cboard[ix][iy] = PROX_WHITE;
                            else CBoard->cboard[ix][iy] = PROX_BLACK;
                        }
                    }
                }
            }

        }
    }
    /* }
 }*/
}