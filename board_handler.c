//
// Created by pookie on 20/11/17.
//

#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include "board_handler.h"
#define DEBUG
static u_int8_t init =0;
static struct board *Board;
void print_board(){
    if(init){
        printf("INFO_: Board view : \n");
        for(int i = 0 ; i < Board->board_size_y ; i++){
            for(int j = 0 ; j < Board->board_size_y ; j++) {
                printf(".%x",Board->board[i*j]);
            }
            printf("\n");
            }
        }
    }
void init_board(){
    if(!init){
    Board = (struct board*)malloc(sizeof(struct board));
    //todo find what is the supposed size
    Board->board_size_x=DEFAULT_BOARD_SIZE_X;
    Board->board_size_y=DEFAULT_BOARD_SIZE_Y;
    Board->lastmove_x = 0;
    Board->lastmove_y =0;
    u_int8_t sizeofboard=(Board->board_size_y*Board->board_size_x);
    Board->board = (u_int8_t*)malloc(sizeof(u_int8_t)*sizeofboard);
    //set to empty
    memset( Board->board, 0 ,sizeof(u_int8_t)*sizeofboard );
    //fill center
    u_int8_t x=(Board->board_size_x -1 );
    u_int8_t y=(Board->board_size_y -1 );
    //set to zero
    memcpy(Board->board,0,sizeof(Board->board));
    //toogle color
    u_int8_t col = 0;
    //set up center
    for(int i = 0 ; i<2; i++){
        for(int j = 0 ; j < 2 ; j ++){
            if(!col)set_board(x-1,y-1,WHITE);
            else set_board(x-1,y-1,BLACK);
            col=!col;
        }
    }
    print_board();

}}
void destroy_board(){
    if(init){
        free(Board->board);
        free(Board);
    }
}
void set_board(u_int8_t x,u_int8_t y , enum CELL val){
    if(x < Board->board_size_x && y < Board->board_size_y){
        Board->board[x*y]= val;
    }else{
        printf("ERRO_:Tying to wright to unused board sector x %u y %u",x,y);
    }
}

u_int8_t get_board_status(u_int8_t x, u_int8_t y){
    if(init==0){
        init_board();
    }
    return Board->board[x*y];
}
u_int8_t* format_board(){
    u_int16_t bsize = Board->board_size_y*Board->board_size_x;
    u_int8_t *formated = (u_int8_t*)malloc(sizeof(u_int8_t)*bsize/4);
    memcpy(formated,0,sizeof(u_int8_t)*bsize/4);
    for(u_int8_t i = 0 ; i < bsize; i++){
        formated[i/4] |= (Board->board[i] >> (i%4));
    }
#ifdef DEBUG
    printf("INFO_:Resulting board : \n");
    for(u_int8_t j = 0 ; j < (bsize /4) ; j++){
        printf("%o ",formated[j]);
    }
    printf("\n");
#endif
    return formated;
}