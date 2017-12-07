//
// Created by pookie on 17/11/17.
//

#include "client_registration.h"
#include <stdio.h>
#include <pthread.h>

/*
 * 0x00 - no client by that identifier is currently connected
 * 0x01 - client is connceted and using info
 * 0x02 - client is connected and info can be accesed without problem
 */
typedef enum {UNDEFINED = 0x00,/* INUSE = 0x01, */DEFINED=0x02}CLIENT_STATUS ;
/*client by id
 * 0 - controller
 * 1 - black
 * 2 - wight
*/
static CLIENT_STATUS client_list[MAX_CLIENT_NUMBER] = {UNDEFINED,UNDEFINED,UNDEFINED};
static pthread_mutex_t lock;
//This is ugly todo
CLIENT_LIST register_new_client(u_int8_t * name,u_int8_t length){
    pthread_mutex_lock(&lock);
    printf("INFO_:Register new client name \n");
    CLIENT_LIST new = ERROR;
     if((length>=2)&&(name[0]=='G'&&name[1]=='C')){
        new = GC;
         client_list[GC]=DEFINED;
    }else{
         if(client_list[BP]==UNDEFINED){
             new = BP;
             client_list[BP]=DEFINED;
         }else {
             if (client_list[WP] == UNDEFINED){
                 new = WP;
                 client_list[WP]=DEFINED;
             }
             else printf("ERRO_:!No client is free, all clients are taken! \n");
         }
     }
    pthread_mutex_unlock(&lock);
        //if(put_nv_client_reg(new)!=VALIDE_REGISTER)new=ERROR;
    return new;
}
/*
CLIENT_ACCES_ERROR get_acces_to_client(CLIENT_LIST cname){
    CLIENT_ACCES_ERROR ret =ACCES_UNDEFINED_CLIENT;
    if(cname!=ERROR){
        if(client_list[cname]==DEFINED){
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
    return ret;
}*/
void unregister_client(CLIENT_LIST cname){
    pthread_mutex_lock(&lock);
    if(client_list[cname]== DEFINED){
        client_list[cname]=UNDEFINED;
        printf("INFO_:Sucessfully unregistered client %x",cname);
    }
    pthread_mutex_unlock(&lock);
}
/*
CLIENT_ACCES_ERROR return_acces_to_client(CLIENT_LIST cname){
    CLIENT_ACCES_ERROR ret =get_acces_to_client(cname);
    if(ret == ACCES_IN_USE){
        client_list[cname] = DEFINED;
    }
    return ret;
}*/
u_int8_t has_all_clients(){
    u_int8_t ret =1;
    for(int i = 0 ; i <MAX_CLIENT_NUMBER; i++){
        if(client_list[i]==UNDEFINED)ret = 0;
    }
   // printf("INFO_:Has all clients %u\n",ret);
    puts(".");
    return ret;
}//check if all necessary clients are present
