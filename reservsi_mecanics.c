//
// Created by pookie on 02/12/17.
//
#include <stdio.h>
#include "board_handler.h"
#include "reservsi_mecanics.h"
#include "message_defines.h"
#include "message_helper.h"

static enum CELL turn;
#define STARTING_PLAYER ((enum CELL)WHITE)
/*Decalrations
 * Private functions
 */
static u_int8_t check_can_player_play(enum CLIENT_LIST player);
static u_int8_t check_move_valide(u_int8_t player,int8_t x , int8_t y);
static u_int8_t flip(u_int8_t joueur,int8_t ix,int8_t iy,int8_t dx,int8_t dy);
/*Definitions
 * Private functions
 */
/*
 * Return 1 - yes
 *        0 - no skip turn
 */
static u_int8_t check_can_player_play(enum CLIENT_LIST player){
    //todo implement to check if client is not stuck
    return 1;
}
/*
 * Returns 1 - yes move is valide
 *         0 - nope you fail
 */
static u_int8_t check_move_valide(u_int8_t player,int8_t x , int8_t y){
    u_int8_t nmbrflips = 0;
    if(x<=get_size_x()&&y<=get_size_y()&&x>=0&& y>=0){
        nmbrflips +=flip(player,x,y,-1,-1);
        nmbrflips +=flip(player,x,y,-1,0);
        nmbrflips +=flip(player,x,y,-1,+1);
        nmbrflips +=flip(player,x,y,0,-1);
        nmbrflips +=flip(player,x,y,0,+1);
        nmbrflips +=flip(player,x,y,+1,-1);
        nmbrflips +=flip(player,x,y,+1,0);
        nmbrflips +=flip(player,x,y,+1,+1);
    }
    printf("INFO_:Player move , x %x , y %x validity %x\n",x,y,nmbrflips);
    return nmbrflips;
}
static u_int8_t flip(u_int8_t joueur,int8_t ix,int8_t iy,int8_t dx,int8_t dy){
    u_int8_t possibilite =0;
    int8_t i = ix + dx;
    int8_t j = iy+dy;
    enum CELL adversaire = get_couleur_adverse(joueur);
    enum CELL coljoueur = get_couleur_joueur(joueur);
    enum CELL etudier;
    while ((i >= 0) && (i < get_size_x() ) && (j >= 0) && (j < get_size_y())){
        etudier = show_at_value(i,j);
        if (adversaire == etudier){ // The bit belongs to the opponent
            possibilite++;
        //filp la case
        set_board(i,j,coljoueur);}
        else if (coljoueur == etudier) // The bit belongs to the player
            return possibilite; // All possible flips become real flips
        else // The bit belongs to no player
            return 0; // There are no possible flips
        i += dx; // Advance in direction
        j += dy; // Advance in direction
    }
}
/*
 * Public functions
 */

void init_game(){
    turn =STARTING_PLAYER;
    init_board();
}
//todo implement
/*
 * This function is called whenever we get a new move from player,
 * it returns the status of the move
 * - ok   0x01
 * - nok  0x00
 */
u_int8_t check_player_move(int socket,enum CLIENT_LIST player,struct comm_message * recvmsg){
    u_int8_t oknok = 0x00;//not okay by default
    int8_t x,y;
    x=recvmsg->msg[0];
    y=recvmsg->msg[1];
    //send comm to player
    if(check_move_valide( player , x,y)){
        oknok=0x01;
        register_new_player_move(player,x,y);
    }
    msg_player_ok(oknok,socket);
    return oknok;
}
void end_game(){
    destroy_board();
}
/*
 *
 * Creats a new move for a player, additionnaly tells it
 * if he must skip his turn
 */
u_int8_t new_move_for_player(int socket,enum CLIENT_LIST player){
    u_int8_t can_play=check_can_player_play(player) ;
    struct comm_message* newmsg = (struct comm_message*)malloc(sizeof(struct comm_message));
    newmsg->msg=board_prepare_msg();
    //todo verifiy might be not +4
    newmsg->mesg_lng=get_board_string_lng()+4;
    if(can_play!=0){
        newmsg->type=NEXT_TURN;
        turn=get_couleur_joueur(player);
    }else{
        newmsg->type=SKIP_TURN;
        turn= get_couleur_adverse(player);
        //don't update turn not the right turn
    }
    sendof(socket,newmsg);
    return can_play;
}
/*
 * Send status message to controller
 * curently supported message :
 *      -status1 x
 *      -status2 nope
 */
u_int8_t send_status(int socket) {
    struct comm_message *nvmsg = (struct comm_message *) malloc(sizeof(struct comm_message));
    nvmsg->type = STATUS_1;
    nvmsg->msg = board_prepare_msg();
    nvmsg->mesg_lng =get_board_msg_size();
    sendof(socket,nvmsg);
}
void send_game_end(int socket){
    struct comm_message *nvmsg = (struct comm_message *) malloc(sizeof(struct comm_message));
    nvmsg->type = END_MSG;
    nvmsg->msg = NULL;
    nvmsg->mesg_lng =0;
    sendof(socket,nvmsg);
}