//
// Created by pookie on 17/11/17.
//

#include "client_registration.h"
#include <stdio.h>
/*
 * 0x00 - no client by that identifier is currently connected
 * 0x01 - client is connceted and using info
 * 0x02 - client is connected and info can be accesed without problem
 */
typedef enum CLIENT_STATUS {UNDEFINED = 0x00, INUSE = 0x01, FREE=0x02};
/*client by id
 * 0 - controller
 * 1 - black
 * 2 - wight
 */
static enum CLIENT_STATUS client_list[MAX_CLIENT_NUMBER] = {UNDEFINED,UNDEFINED,UNDEFINED};
enum CLIENT_REGISTER_ERROR put_nv_client_reg(enum CLIENT_LIST nv){
    printf("INFO_:New client id %u\n",nv);
    if(client_list[nv]==UNDEFINED){
        client_list[nv]=FREE;
        return VALIDE_REGISTER;
    }else{
        printf("ERRO_:Client is already registered value of "
                       "register is %u error code %u\n",client_list[nv],DUPLICATE_CLIENT);
        return DUPLICATE_CLIENT;
    }
}
enum CLIENT_LIST register_new_client(u_int8_t * name){
    printf("INFO_%d:Register new client name %c %c\n",name[0],name[1]);
    enum CLIENT_LIST new = ERROR;
     if(name[0]=='G'&&name[1]=='C'){
        new = GC;
    }else{
         if(client_list[BP]==UNDEFINED){
             new = BP;
         }else new = WP;
     }
        if(put_nv_client_reg(new)!=VALIDE_REGISTER)new=ERROR;

    return new;
}
enum CLIENT_ACCES_ERROR get_acces_to_client(enum CLIENT_LIST cname){
    enum CLIENT_ACCES_ERROR ret =ACCES_UNDEFINED_CLIENT;
    if(cname!=ERROR){
        if(client_list[cname]==FREE){
            ret = VALIDE_ACCES;
        }else{
            //error in unregistration
            //if(client_list[cname]==INUSE)
            ret = ACCES_IN_USE; // only possiblility
        }
    }
#ifdef DEBUG
    if(ret!= VALIDE_ACCES)printf("ERRO_:Acces client error %x ",ret);
#endif
}
enum CLIENT_ACCES_ERROR unregister_client(enum CLIENT_LIST cname){
    enum CLIENT_ACCES_ERROR ret =get_acces_to_client(cname);
    if(ret == FREE){
        client_list[cname]=UNDEFINED;
        printf("INFO_:Sucessfully unregistered client %x",cname);
    }
    return ret;
}

enum CLIENT_ACCES_ERROR return_acces_to_client(enum CLIENT_LIST cname){
    enum CLIENT_ACCES_ERROR ret =get_acces_to_client(cname);
    if(ret == ACCES_IN_USE){
        client_list[cname] = FREE;
    }
    return ret;
}
u_int8_t has_all_clients(){
    u_int8_t ret =1;
    for(int i = 0 ; i <MAX_CLIENT_NUMBER; i++){
        if(client_list[i]==UNDEFINED)ret = 0;
    }
   // printf("INFO_:Has all clients %u\n",ret);
    puts(".");
    return ret;
}//check if all necessary clients are present
