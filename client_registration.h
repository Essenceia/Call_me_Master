//
// Created by pookie on 17/11/17.
//

#include <stdlib.h>

#ifndef CPROJ_CLIENT_REGISTRATION_H
#define CPROJ_CLIENT_REGISTRATION_H
#define MAX_CLIENT_NUMBER 3
typedef enum {WP = 0x02 , BP = 0x01, GC = 0x02,ERROR = 0xff}CLIENT_LIST;
typedef enum {VALIDE_REGISTER = 0,DUPLICATE_CLIENT = -1}CLIENT_REGISTER_ERROR;
CLIENT_LIST register_new_client(u_int8_t * name,u_int8_t length);
void unregister_client(CLIENT_LIST cname);
//CLIENT_ACCES_ERROR get_acces_to_client(CLIENT_LIST cname);
//CLIENT_ACCES_ERROR return_acces_to_client(CLIENT_LIST cname);
u_int8_t has_all_clients();//check if all necessary clients are present

#endif //CPROJ_CLIENT_REGISTRATION_H
