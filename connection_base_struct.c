//
// Created by pookie on 16/11/17.
//

#include "time_keeper.h"
#include "connection_base_struct.h"
#include "lib_connection.h"
#include <stdio.h>
#include <memory.h>
#include "message_defines.h"
#include <unistd.h>
#include "gams_status.h"
#include "message_parser.h"

u_int8_t run_connection(connection_base victim);
/*
 * Create new connection base structure - initialise a connection and find
 * witch type of client we are addressing.
 *
 */
void init_by_type(connection_base *toinit){
    //store name in name char
   printf("INFO_%d:New client created with NAME %s",getpid(),(char *)toinit->rev_msg->msg+1);
    if((toinit->client_type == WP )||(toinit->client_type==BP)){
        set_player_name(toinit->client_type,(u_int8_t )toinit->rev_msg->mesg_lng-1,(toinit->rev_msg->msg+1));
        toinit->handler = &player_handler;

    }else{
        toinit->clock->time_to_warn=CONTROLER_TIME_WAIT;
        toinit->handler = &gclient_handler;
    }
}
void default_warn(){
    printf("INFO_%d:Default time has elapsed no connection recived from client\n",getpid());
}
/*
 * Init the connection_base structure with default values : we don't know how we are talking to yet.
 * Possible candidates : BP , WP , GC
 * We will have to change the default message warining handler and the wait
 * times accordingly.
 */
connection_base* init_connection(int dest_sochket){
    connection_base* returnstruct, *cbase = (connection_base*)malloc(sizeof(connection_base));
    returnstruct = NULL;
    //default init
    printf("INFO_%d_:Connection init start",getpid());
    cbase->alive = DEAD;
    cbase->rev_msg = NULL;
    cbase->handler = NULL;
    cbase->dest_socket = dest_sochket;
    //check untill max wait time has elapsed
    //while (cbase->clock->check_elapsed_time(cbase->clock)){
        printf("INFO_%d_:Wating for first client message\n",getpid());
        cbase->rev_msg = recive_msg(cbase->dest_socket);
        if(cbase->rev_msg != NULL){
            printf("INFO_%d_:we have recived a message\n",getpid());
            //we have recived a message
            //check if it is a connection request
            if(cbase->rev_msg->type == CONNECT){
                //register the client
                printf("INFO_%d_:Incoming connection request , going to register the client\n",getpid());
                cbase->client_type = register_new_client(cbase->rev_msg->msg,cbase->rev_msg->mesg_lng-1);
                if(cbase->client_type!=ERROR){
                    printf("INFO_%d_:we have a valide new client, type %x\n",getpid(),cbase->client_type);
                    //we have a valide new client
                    cbase->alive = ALIVE;
                    //initialise the client to it's final status
                    init_by_type(cbase);
                    returnstruct = cbase;
                }



            }else{
                printf("WARN_%d:Message recived from client is of the wrong type"
                             "expecting connect 0x01 but got %x ",getpid(),cbase->rev_msg->type);
            }
            //destroy message
            destroy_msg(cbase->rev_msg);

        }else{
            printf("INFO_%d:Error message was invalide\n",getpid());
        }
    //}
    if(returnstruct == NULL ) destroy_connection_base(cbase);
    return returnstruct;

}
/*
 * Destroy the connection base structure
 * to be done :
 *  -free TimeKeeper x
 *  -free comm msg x
 *  -declare client as undefined x
 *  -free client name char x
 *  -close socket x
 */
void destroy_connection_base(connection_base *tokill){
    if(tokill->alive){
    if(unregister_client(tokill->client_type)!=VALIDE_ACCES)puts("ERRO_:Error in client unregistering");
    free(tokill->clock);
    destroy_msg(tokill->rev_msg);
    destroy_msg(tokill->tosend_msg);
    close(tokill->dest_socket);
    //final blow
    free(tokill);
    printf("INFO_:Client struct was destroyed");
    }else puts("ERRO_:Connection should be set as dead");
}

