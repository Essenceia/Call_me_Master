//
// Created by pookie on 16/11/17.
//

#ifndef CPROJ_HIDDEN_SERVER_HELPERS_H
#define CPROJ_HIDDEN_SERVER_HELPERS_H

#include <stdlib.h>
#include <time.h>
#define DEFAULT_TIME_WAIT ((u_int8_t) 10)
#define DEFAULT_TIME_DIE ((u_int8_t) 15)
#define CONTROLER_TIME_WAIT ((u_int8_t) 20)
#define CONTROLER_TIME_DIE ((u_int8_t) 25)
typedef void (*reset_func ) (void*);
typedef void (*warn_func ) (void);
typedef u_int8_t (*ttl_kill_func ) (void*);
typedef void (*seter) (void*);
typedef struct TimeKeeper{
    u_int8_t active; // boolean is timer active ?
    time_t time_to_warn;//time untill raise warning
    time_t time_to_die;//time a witch we die if nothing happens
    const u_int8_t warn_delay_increment;//incrementation
    const u_int8_t die_delay_increment;//incrementation
    reset_func reset;
    warn_func warn_timeout;
    ttl_kill_func check_elapsed_time;
    seter start_timer;
    seter stop_timer;
};

struct TimeKeeper* init_TimeKeeper(u_int8_t winc,u_int8_t dinc,void (*warnfunc)(void));
#endif //CPROJ_HIDDEN_SERVER_HELPERS_H
