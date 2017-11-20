//
// Created by pookie on 16/11/17.
//

#ifndef CPROJ_CONNECTION_BASE_STRUCT_H
#define CPROJ_CONNECTION_BASE_STRUCT_H
/*
 * definition of the basic connection structure
 */
#include "time_keeper.h"
#include "message_parser.h"
#include "client_registration.h"
typedef enum CONNECTION_LIFE {ALIVE=0xff, DEAD=0x00};
typedef void (*msg_handler)(void*,int);
static u_int8_t GAME_OVER;
typedef struct connection_base{
    struct TimeKeeper *clock;
    int dest_socket;//destination socket
    struct comm_message* rev_msg;
    char *client_name;
    enum CLIENT_LIST client_type;
    enum CONNECTION_LIFE alive;
    msg_handler handler;
};

u_int8_t run_connection(struct connection_base victim);

struct connection_base* init_connection(int dest_sochket);
void destroy_connection_base(struct connection_base *tokill);
#endif //CPROJ_CONNECTION_BASE_STRUCT_H
