//
// Created by pookie on 06/12/17.
//

#include <pthread.h>
#include "game_step.h"
static u_int16_t game_step =0;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void incr_game_step(){
    pthread_mutex_lock(&lock);
    game_step++;
    pthread_mutex_unlock(&lock);

}
u_int16_t get_game_step(){
    u_int16_t retval;
    pthread_mutex_lock(&lock);
    retval = game_step;
    pthread_mutex_unlock(&lock);
    return retval;
}
