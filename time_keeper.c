//
// Created by pookie on 16/11/17.
//
#include "time_keeper.h"
#include <stdio.h>
#include <pthread.h>
#define DEFAULT_WAIT_TIME 300
#define DEBUG
#define PINX(p) ((p)==BP)?0:1
static unsigned char init = 0;
static TimeKeeper timers[2];
void init_timer(){
    for(int i = 0 ; i < 2 ; i ++){
        timers[i].warn_delay_increment = DEFAULT_WAIT_TIME;
        timers[i].active = 0;
        timers[i].time_to_warn = 0;
    }
    init ++;
}
TIMER_STATUS  timer_check_elapsed_time(CLIENT_LIST player){
    time_t current = time(NULL);
#ifdef DEBUG
    printf("INFO_%d:Cheching time current %lu\n    active %x waring time %lu \n",pthread_self(),(unsigned long)current,timers[PINX(player)].active,(unsigned long)timers[PINX(player)].time_to_warn);

#endif
    if(timers[PINX(player)].active){


        if(current>= timers[PINX(player)].time_to_warn){
#ifdef DEBUG
            printf("INFO_%d: Timer status TIMER_OVERFLOW\n ");
#endif
            return TIMER_OVERFLOW;
        }
        else{
#ifdef DEBUG
            printf("INFO_%d: Timer status TIMER_COUNTING\n ");
#endif
            return TIMER_COUNTING;
        }
    }else{
        return TIMER_OFF;
    }
}
void timer_start(CLIENT_LIST player){
    if(!init)init_timer();
    timers[PINX(player)].active=1;
    timers[PINX(player)].time_to_warn = time(NULL) + timers[PINX(player)].warn_delay_increment;
}
void timer_stop(CLIENT_LIST player){
    timers[PINX(player)].active=0;
}
//todo tel of the remaining time in timer
u_int8_t timer_get_remaing(CLIENT_LIST player){
    u_int8_t remaing = CONTROLER_TIME_WAIT;
    if(timers[PINX(player)].active){
        remaing = (u_int8_t )((0xFF) & (timers[PINX(player)].time_to_warn - time(NULL)));
    }
    return remaing;
}