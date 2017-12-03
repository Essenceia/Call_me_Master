//
// Created by pookie on 03/12/17.
//

#include <memory.h>
#include "gams_status.h"
#include "board_handler.h"

static game_status game;
void add_point(enum CLIENT_LIST player,u_int8_t points){
    if(player==WP)game.points_WP+=points;
    else game.points_BP+=points;
}
u_int8_t is_game_over(){
    return ((game.GAME_OVER || (game.stuck_BP && game.stuck_WP))?1:0);
}
void set_game_over(){
    game.GAME_OVER=1;
}
void set_stuck_status(enum CLIENT_LIST player,u_int8_t is_stuck){
    if(player==WP)game.stuck_WP=is_stuck;
    else game.stuck_BP=is_stuck;
}
void init_game_status(){
    game.GAME_OVER=0;
    game.points_BP=0;
    game.points_WP=0;
    game.stuck_BP=0;
    game.stuck_WP=0;
    game.name_BP_length=0;
    game.name_WP_length=0;
}
void destroy_game_status(){
    if(game.name_BP_length)free(game.name_BP);
    if(game.name_WP_length)free(game.name_WP);
}
void set_player_name(enum CLIENT_LIST player,u_int8_t lenght,u_int8_t * name){
    u_int8_t *nvname = (u_int8_t *)malloc(sizeof(u_int8_t)*lenght);
    memcpy(nvname,name,sizeof(char)*lenght);
    if(player==WP){
        game.name_WP = nvname;
        game.name_WP_length = lenght;
    }else{
        game.name_BP = nvname;
        game.name_BP_length = lenght;
    }
}
u_int8_t get_points(enum CLIENT_LIST player){
    return (player==WP)?game.points_WP:game.points_BP;
}
u_int8_t *get_name(enum CLIENT_LIST player){
   return (player==WP)?game.name_WP:game.name_BP;

}
u_int8_t get_name_length(enum CLIENT_LIST player){
    return (player==WP)?game.name_WP_length:game.name_BP_length;
}
u_int8_t get_status_length(){
    return game.name_BP_length+game.name_WP_length+8;
}
