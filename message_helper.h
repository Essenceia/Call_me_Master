//
// Created by pookie on 17/11/17.
//

#ifndef CPROJ_MESSAGE_HELPER_H
#define CPROJ_MESSAGE_HELPER_H
#include <stdlib.h>
char check_valide_legth(u_int8_t tmsglng, u_int8_t msgl);
char check_type_valide(char type);
u_int8_t get_crc(u_int8_t *appmsg,size_t alng);
u_int8_t check_error(u_int8_t *recvmsg,u_int8_t recvlng);
u_int8_t *prepare_message(u_int8_t type,u_int8_t *msg,u_int8_t length);
u_int8_t* msg_ok(u_int8_t type);
#endif //CPROJ_MESSAGE_HELPER_H
