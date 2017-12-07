//
// Created by pookie on 02/12/17.
//
#include <stdio.h>
#include <memory.h>
#include <pthread.h>
#include "board_handler.h"
#include "reservsi_mecanics.h"
#include "message_defines.h"
#include "message_helper.h"
#include "gams_status.h"
#include "message_parser.h"

#define DEBUG
//#define DEBUG_2
#define DEBUG_3
static CLIENT_LIST turn;
#define STARTING_PLAYER ((CLIENT_LIST)WP)
/*Decalrations
 * Private functions
 */
static u_int8_t check_can_player_play(CLIENT_LIST player);
static u_int8_t check_move_valide(CLIENT_LIST player,int8_t x , int8_t y);
static u_int8_t check_minim_move_valide(CLIENT_LIST player,int8_t x , int8_t y);
static u_int8_t flip(u_int8_t do_flip,CLIENT_LIST player,int8_t ix,int8_t iy,int8_t dx,int8_t dy);
/*Definitions
 * Private functions
 */
/*
 * Return 1 - yes
 *        0 - no skip turn
 */
static u_int8_t check_can_player_play(CLIENT_LIST player){
    //todo implement to check if client is not stuck
    u_int8_t retval= 0;
    CELL adverary = get_couleur_adverse(player);
#ifdef DEBUG_3
    printf("INFO_%d:Checking if player %x, with col %x has an space he can play on, adversary col %x \n",pthread_self(),player,get_couleur_joueur(player), adverary);
#endif

    for(int8_t x = 0 ; x < get_size_x(); x ++){
        for(int8_t y = 0 ; y < get_size_y(); y ++){
            if(has_prox_col(adverary,x,y)){
                //check if move would be valide
#ifdef DEBUG_3
                printf("INFO_%d:Found adversaty neigbour on pos x:%x y:%x\n",pthread_self(),x,y);
#endif
                if(check_minim_move_valide(player,x,y)){
                    retval =1;//if true break and return retval
                    goto ckecked;
                }

            }
        }
    }
    ckecked:
    return retval;
}
static u_int8_t check_minim_move_valide(CLIENT_LIST player,int8_t x , int8_t y){
    u_int8_t valide =0;
    for (int8_t dx = -1; dx < 2; dx++) {
        for (int8_t dy = -1; dy < 2; dy++) {
            if (!(dx == 0 && dy == 0)) {
        if(flip(0,player,x,y,dx,dy)){
            valide=1;
#ifdef DEBUG_3
    printf("INFO_%d, Found a valide flip x%x y%x\n",pthread_self(),x,y);
#endif
            goto checked;
        }
    }}}
    checked:
    return valide;
}
/*
 * Returns 1 - yes move is valide
 *         0 - nope you fail
 */
static u_int8_t check_move_valide(CLIENT_LIST player,int8_t x , int8_t y){

    u_int8_t nmbrflips = 0;
    u_int8_t scrap_val=0;
    if(has_prox_col(get_couleur_adverse(player),x,y)) {
#ifdef DEBUG
        printf("INFO_:Player move , x %x , y %x starting to verify validity \n",x,y);
#endif
        if (x < get_size_x() && y < get_size_y() && x >= 0 && y >= 0 &&
            (show_at_value((u_int8_t) x, (u_int8_t) y)) ==EMPTY) {
            nmbrflips += flip(0, player, x, y, -1, -1);
            nmbrflips += flip(0, player, x, y, -1, 0);
            nmbrflips += flip(0, player, x, y, -1, +1);
            nmbrflips += flip(0, player, x, y, 0, -1);
            nmbrflips += flip(0, player, x, y, 0, +1);
            nmbrflips += flip(0, player, x, y, +1, -1);
            nmbrflips += flip(0, player, x, y, +1, 0);
            nmbrflips += flip(0, player, x, y, +1, +1);
            if(nmbrflips){
                scrap_val += flip(1, player, x, y, -1, -1);
                scrap_val += flip(1,player, x, y, -1, 0);
                scrap_val += flip(1, player, x, y, -1, +1);
                scrap_val += flip(1, player, x, y, 0, -1);
                scrap_val += flip(1, player, x, y, 0, +1);
                scrap_val += flip(1, player, x, y, +1, -1);
                scrap_val += flip(1, player, x, y, +1, 0);
                scrap_val += flip(1, player, x, y, +1, +1);
            }
        }
    }
#ifdef DEBUG
    printf("INFO_:Player move , x %x , y %x validity %x\n",x,y,nmbrflips);
#endif
    add_point(player,nmbrflips);
    return nmbrflips;
}
static u_int8_t flip(u_int8_t do_flip,CLIENT_LIST joueur,int8_t ix,int8_t iy,int8_t dx,int8_t dy){
    u_int8_t possibilite =0;
    int8_t i = ix + dx;
    int8_t j = iy+dy;

    CELL adversaire = get_couleur_adverse(joueur);
    CELL coljoueur = get_couleur_joueur(joueur);
    CELL etudier;
#ifdef DEBUG
    printf("INFO_:Check if we can flip x:%x y:%x to col %x\n",i,j,coljoueur);
#endif
    while ((i >= 0) && (i < get_size_x() ) && (j >= 0) && (j < get_size_y())){

        etudier = show_at_value((u_int8_t )i,(u_int8_t )j);
#ifdef DEBUG
        printf("INFO_:While loop : Check if we can flip x:%x y:%x to col %x / adversary %x, cell current %x\n",i,j,coljoueur,adversaire,etudier);
#endif
        if (adversaire == etudier){ // The bit belongs to the opponent
            possibilite++;

        //filp la case
#ifdef DEBUG
            printf("INFO_:Found something to flip x:%x y:%x to col %x\n",i,j,coljoueur);
#endif
        if(do_flip)set_board((u_int8_t )i,(u_int8_t )j,coljoueur);}
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
u_int8_t check_player_move(int socket,CLIENT_LIST player,comm_message * recvmsg){
    u_int8_t oknok = 0x00;//not okay by default
    int8_t x,y;
    x=recvmsg->msg[0];
    y=recvmsg->msg[1];
    //send comm to player
    if(check_move_valide( player , x,y)){
        oknok=0x01;
#ifdef DEBUG_2
    printf("INFO_:Le move du playeur est valide, setting up case to x:%x y:%x to col %x \n",x,y,get_couleur_joueur(player));
#endif
        set_board(x,y,get_couleur_joueur(player));
        register_new_player_move(player,x,y);
    }
    msg_oknok(oknok,socket);
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
u_int8_t new_move_for_player(int socket,CLIENT_LIST player){
    u_int8_t can_play=check_can_player_play(player) ;
    set_stuck_status(player,can_play);
#ifdef DEBUG
printf("INFO_:Player %x can play %x\n",player,can_play);
#endif
    comm_message* newmsg = (comm_message*)malloc(sizeof(comm_message));

#ifndef DEBUG
    printf("INFO_%d:New move message length was set to %d , can play %x\n",pthread_self(),newmsg->mesg_lng,can_play);
#endif

    if(can_play!=0){
        newmsg->type=NEXT_TURN;
        turn=player;
    }else{
        newmsg->type=SKIP_TURN;
        turn= get_couleur_adverse(player);
        //don't update turn not the right turn
    }
    newmsg->mesg_lng=get_board_string_lng()+4;
    newmsg->msg=board_prepare_msg();
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
    u_int8_t length=0;
    CLIENT_LIST player = BP;
    comm_message *nvmsg = (comm_message *) malloc(sizeof(comm_message));
    nvmsg->type = STATUS_1;
    nvmsg->msg = board_prepare_msg();
    nvmsg->mesg_lng =get_board_msg_size();
    sendof(socket,nvmsg);
    nvmsg = (comm_message *) malloc(sizeof(comm_message));
    nvmsg->msg=(u_int8_t*)malloc(sizeof(u_int8_t)*get_status_length());
    nvmsg->type=STATIS_2;
    for(u_int8_t i =0 ; i <2 ; i++){
        nvmsg->msg[length++]=get_points(player);
        nvmsg->msg[length++]=30;//todo pass timer ...
        nvmsg->msg[length++]=30;
        nvmsg->msg[length++]=get_name_length(player);
        if(get_name_length(player)){
            memcpy(&nvmsg->msg[length+1],get_name(player),get_name_length(player));
            length+=get_name_length(player);
        }
        player=WP;
    }
    nvmsg->mesg_lng=length;
    sendof(socket,nvmsg);
}
void send_game_end(int socket){
    comm_message *nvmsg = (comm_message *) malloc(sizeof(comm_message));
    nvmsg->type = END_MSG;
    nvmsg->msg = NULL;
    nvmsg->mesg_lng =0;
    sendof(socket,nvmsg);
}
CLIENT_LIST get_turn(){
    return turn;
}