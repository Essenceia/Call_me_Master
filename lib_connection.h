//
// Created by pookie on 12/11/17.
//

#ifndef CPROJ_LIB_CONNECTION_H
#define CPROJ_LIB_CONNECTION_H
#define MAX_CLIENT_MESSAGE_SIZE ((size_t)50)
#define INIT_MESSAGE "HELLO I AM YOUR CONNECTION HANDLER"

#include "message_parser.h"

typedef void (*client_warining) (void);
client_warining get_player_waring();
client_warining get_gcontroler_waring();
void player_handler(void* player);
void gclient_handler(void *gclient);
//void *handler_connection(void* )__attribute_deprecated_msg__("depreciated todo : remove");
//void send_init_message(int socket)__attribute_deprecated_msg__("depreciated todo : remove");
//void handler_message(char *recived_message,int msg_leng, int socket)__attribute_deprecated_msg__("depreciated todo : remove");
comm_message* recive_msg(int socket);
#endif //CPROJ_LIB_CONNECTION_H
