//
// Created by pookie on 17/11/17.
//

#ifndef CPROJ_MESSAGE_HELPER_H
#define CPROJ_MESSAGE_HELPER_H
#include <stdlib.h>
#include "message_parser.h"
char check_valide_legth(u_int8_t tmsglng, u_int8_t msgl);
char check_type_valide(char type);
u_int8_t get_crc(u_int8_t *appmsg,size_t alng);
u_int8_t check_error(u_int8_t *recvmsg,u_int8_t recvlng);
void msg_player_ok(u_int8_t player,int socket);
void msg_oknok(u_int8_t oknok, int socket);
comm_message* prepare_new_move(u_int8_t *msg);
int8_t sendof(int socket , comm_message *msg);
comm_message* recapsulate_for_player(comm_message *to_recapsulate);
comm_message* recapsulate_for_controller(comm_message *sendtoclient, u_int8_t client_id);
#endif //CPROJ_MESSAGE_HELPER_H
