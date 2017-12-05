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
typedef enum  {ALIVE=0xff, DEAD=0x00}CONNECTION_LIFE;
typedef void (*msg_handler)(void*);
typedef struct {
    TimeKeeper *clock;
    int dest_socket;//destination socket
    comm_message* rev_msg;
    comm_message* tosend_msg;
    CLIENT_LIST client_type;
    CONNECTION_LIFE alive;

    msg_handler handler;
}connection_base;


connection_base* init_connection(int dest_sochket);
void destroy_connection_base(connection_base *tokill);
#endif //CPROJ_CONNECTION_BASE_STRUCT_H
