//
// Created by pookie on 17/11/17.
//

#include <stdlib.h>

#ifndef CPROJ_CLIENT_REGISTRATION_H
#define CPROJ_CLIENT_REGISTRATION_H
#define MAX_CLIENT_NUMBER 3
#define CLIENT_NAME_LENGTH 2
typedef enum CLIENT_LIST {WP = 0x02 , BP = 0x01, GC = 0x02,ERROR = 0xff};
typedef enum CLIENT_REGISTER_ERROR {VALIDE_REGISTER = 0,DUPLICATE_CLIENT = -1,INVALIDE_NAME = -2};
typedef enum CLIENT_ACCES_ERROR {VALIDE_ACCES = 0 , ACCES_IN_USE= -1,ACCES_UNDEFINED_CLIENT = -2};
enum CLIENT_LIST register_new_client(u_int8_t * name);
enum CLIENT_ACCES_ERROR unregister_client(enum CLIENT_LIST cname);
enum CLIENT_ACCES_ERROR get_acces_to_client(enum CLIENT_LIST cname);
enum CLIENT_ACCES_ERROR return_acces_to_client(enum CLIENT_LIST cname);
u_int8_t has_all_clients();//check if all necessary clients are present

#endif //CPROJ_CLIENT_REGISTRATION_H
