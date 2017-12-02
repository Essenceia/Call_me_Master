//
// Created by pookie on 02/12/17.
//
#include "board_handler.h"
#include "reservsi_mecanics.h"

void init_game(){
    init_board();
}
//todo implement
/*
 * This function is called whenever we get a new move from player,
 * it returns the status of the move
 * - ok
 * - nok
 */
struct comm_message* check_player_move(u_int8_t player,u_int8_t x,u_int8_t y){

}
void end_game(){
    destroy_board();
}
/*
 * Todo implement
 * Creats a new move for a player, additionnaly tells it
 * if he must skip his turn
 */
struct comm_message* new_move_for_player(u_int8_t player){

}