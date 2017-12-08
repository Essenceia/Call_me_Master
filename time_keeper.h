//
// Created by pookie on 16/11/17.
//

#ifndef CPROJ_HIDDEN_SERVER_HELPERS_H
#define CPROJ_HIDDEN_SERVER_HELPERS_H

#include <stdlib.h>
#include <time.h>
#include "client_registration.h"

#define CONTROLER_TIME_WAIT ((time_t) 30)
typedef enum{ TIMER_OFF = 0, TIMER_COUNTING = 1, TIMER_OVERFLOW =2}TIMER_STATUS;
typedef struct{
    u_int8_t active; // boolean is timer active ?
    time_t time_to_warn;//time untill raise warning
    //time_t time_to_die;//time a witch we die if nothing happens
    time_t warn_delay_increment;//incrementation
    //const u_int8_t die_delay_increment;//incrementation

}TimeKeeper;

//struct TimeKeeper* init_TimeKeeper(u_int8_t winc,u_int8_t dinc,void (*warnfunc)(void));
//warn_func warn_timeout;
TIMER_STATUS timer_check_elapsed_time(CLIENT_LIST player);
void timer_start(CLIENT_LIST player);
void timer_stop(CLIENT_LIST player);
u_int8_t get_elapsed(CLIENT_LIST player);
u_int8_t timer_get_remaing(CLIENT_LIST player);
#endif //CPROJ_HIDDEN_SERVER_HELPERS_H
