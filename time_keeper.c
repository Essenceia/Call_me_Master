//
// Created by pookie on 16/11/17.
//
#include "time_keeper.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define DEBUG
TimeKeeper* init_TimeKeeper(time_t wait_time){
    //I want my const value \(+ O +)/
    TimeKeeper tmpstruct = { .warn_delay_increment = wait_time,.active = 0, .time_to_warn = 0 };
    TimeKeeper *nvt=(TimeKeeper*)malloc(sizeof(TimeKeeper));
    memcpy(nvt,&tmpstruct,sizeof(TimeKeeper));
    return nvt;
}
enum TIMER_STATUS timer_check_elapsed_time(TimeKeeper *t){
    if(t->active){
        time_t current = time(NULL);
#ifdef DEBUG
        printf("INFO_%d:Cheching time current %d , waring time %d \n",getpid(),current,t->time_to_warn);
#endif
        if(current>= t->time_to_warn)return TIMER_OVERFLOW;
        else return TIMER_COUNTING;
    }else{
        return TIMER_OFF;
    }
}
void timer_start(TimeKeeper *t){
    t->active=1;
    t->time_to_warn = time(NULL) + t->warn_delay_increment;
}
void timer_stop(TimeKeeper *t){
    t->active=0;
}
u_int8_t timer_get_remaing(TimeKeeper *t){
    u_int8_t remaing = CONTROLER_TIME_WAIT;
    if(t->active){
        remaing = (u_int8_t )((0xFF) & (t->time_to_warn - time(NULL)));
    }
    return remaing;
}