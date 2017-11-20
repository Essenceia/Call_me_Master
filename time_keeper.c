//
// Created by pookie on 16/11/17.
//
#include "time_keeper.h"
#include <stdio.h>
#include <string.h>
#define DEFAULT_DIE_SUPLEMENT 1
#define DEBUG
#define DEFAULT_TIMER_ACTIVATION 1
void start_timer(struct TimeKeeper *stime){//todo - why do you set to unactive ?
#ifdef DEBUG
    printf("INFO_:Timer is beeing started at time %d\n",(int)time(NULL));
#endif
    memset(stime->active,0xFF,sizeof(stime->active));
    //stime->active = 1;
 //   stime->reset(stime);
}
void stop_timer(struct TimeKeeper *stime){
    stime->active = 0;
}
void reset_timeout(struct TimeKeeper *stime){
    //reset timers
    stime->time_to_warn = time(NULL) + stime->warn_delay_increment;
    stime->time_to_die = time(NULL) + stime->die_delay_increment;

}
u_int8_t should_die(struct TimeKeeper *stime){
    u_int8_t retval = 1; // keep alive

    if(stime->active == 1) {
        time_t now = time(NULL);
#ifdef DEBUG
        printf("INFO_:Checking time , now : %d war : %d die : %d\n",now,time(NULL)+stime->time_to_warn,time(NULL)+stime->time_to_die);
#endif
        if (now >= stime->time_to_warn) {
            //we have to send warning
            warn_func func;
            func = stime->warn_timeout;
            (*func)();
            if (now >= stime->time_to_die) {
                retval = 0; //kill
#ifdef DEBUG
                printf("WARN_:Max stime elasped , going to die, time %d\n", (int) now);
#endif
            }
        }
    }
#ifdef DEBUG
    puts("INFO_:Out of timerfunc\n");
#endif
    return retval;
}


struct TimeKeeper* init_TimeKeeper(u_int8_t winc,u_int8_t dinc,void (*warnfunc)(void)){
    struct TimeKeeper* stime=(struct TimeKeeper*)malloc(sizeof(struct TimeKeeper));
    time_t now = time(NULL);
    stime->active =DEFAULT_TIMER_ACTIVATION;
    printf("INFO_:By default timer activation state is %s",(DEFAULT_TIMER_ACTIVATION? "active" : "unactive"));
    stime->time_to_warn = now + winc;
    if(winc >= dinc){
#ifdef DEBUG
        printf("WARN_:TimeKeeper , warning delay smaller than die delay, die delay will be overwriten "
                     "with default value.  time : %d\n",(int)now);
#endif
        dinc = winc + DEFAULT_DIE_SUPLEMENT;
    }
    stime->time_to_die = now+dinc;
    puts("3");
    stime->reset = &reset_timeout;
    puts("4");
    stime->check_elapsed_time = &should_die;
    stime->start_timer = &start_timer;
    stime->start_timer = &stop_timer;
    puts("5");
    stime->warn_timeout = warnfunc;
    puts("6");

    return stime;

}
