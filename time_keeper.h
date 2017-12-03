//
// Created by pookie on 16/11/17.
//

#ifndef CPROJ_HIDDEN_SERVER_HELPERS_H
#define CPROJ_HIDDEN_SERVER_HELPERS_H

#include <stdlib.h>
#include <time.h>
#define CONTROLER_TIME_WAIT ((time_t) 30)
typedef enum TIMER_STATUS{ TIMER_OFF = 0, TIMER_COUNTING = 1, TIMER_OVERFLOW =2};
typedef struct{
    u_int8_t active; // boolean is timer active ?
    time_t time_to_warn;//time untill raise warning
    //time_t time_to_die;//time a witch we die if nothing happens
    const time_t warn_delay_increment;//incrementation
    //const u_int8_t die_delay_increment;//incrementation

}TimeKeeper;

//struct TimeKeeper* init_TimeKeeper(u_int8_t winc,u_int8_t dinc,void (*warnfunc)(void));
TimeKeeper* init_TimeKeeper(time_t wait_time);
//warn_func warn_timeout;
enum TIMER_STATUS timer_check_elapsed_time(TimeKeeper *t);
void timer_start(TimeKeeper *t);
void timer_stop(TimeKeeper *t);
u_int8_t timer_get_remaing(TimeKeeper *t);
#endif //CPROJ_HIDDEN_SERVER_HELPERS_H
