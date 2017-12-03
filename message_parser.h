//
// Created by pookie on 17/11/17.
//

#ifndef CPROJ_MESSAGE_PARSER_H
#define CPROJ_MESSAGE_PARSER_H

#include <stdlib.h>

typedef struct{
    char type;
    size_t mesg_lng ;
    u_int8_t * msg;

}comm_message;
comm_message* parse_recv_msg(u_int8_t * recvmsg, ssize_t recvlngth);
void destroy_msg(comm_message *tokill);
#endif //CPROJ_MESSAGE_PARSER_H
