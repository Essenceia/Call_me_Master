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
//prepare_message(u_int8_t type,u_int8_t *msg,u_int8_t length);
//void msg_player_ok(u_int8_t type, int socket);
//void msg_ok_nok(u_int8_t socket,u_int8_t oknok);
void msg_player_ok(u_int8_t oknok,int socket);
//void next_turn(int socket);
//u_int8_t *prepare_new_move(u_int8_t *msg);
struct comm_message* prepare_new_move(u_int8_t *msg);
//int8_t sendof(int socket , u_int8_t *msg,u_int8_t length);
int8_t sendof(int socket , struct comm_message *msg);
struct comm_message* recapsulate_for_player(struct comm_message *to_recapsulate);
struct comm_message* recapsulate_for_controller(struct comm_message *sendtoclient, u_int8_t client_id);
#endif //CPROJ_MESSAGE_HELPER_H
