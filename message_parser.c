//
// Created by pookie on 17/11/17.
//
#include "message_parser.h"
#include <stdio.h>
#include <memory.h>
#include "message_helper.h"
#include "message_defines.h"
#include <unistd.h>
#define DEBUG
struct comm_message* parse_recv_msg(u_int8_t * recvmsg, ssize_t recvlngth){
#ifdef DEBUG
    printf("INFO_%d: Recived message length %u: \n",getpid(),recvlngth);
    for(int i = 0 ; i < recvlngth; i++){
        printf("_%c(%u)",recvmsg[i],recvmsg[i]);
    }
    printf("\n");
#endif
    struct comm_message *nvmsg=NULL;
    u_int8_t error = check_error(recvmsg,recvlngth);
    if(error>=0) {
        nvmsg = (struct comm_message *) malloc(sizeof(struct comm_message));
        nvmsg->type =(u_int8_t) recvmsg[OFFSET_TYPE];
        nvmsg->mesg_lng = (u_int8_t )recvmsg[OFFSET_LNGT];
        nvmsg->msg = (u_int8_t *)malloc(sizeof(u_int8_t)*nvmsg->mesg_lng);
        for(int i = 0 ; i < nvmsg->mesg_lng; i++){
            nvmsg->msg[i] = recvmsg[OFFSET_TYPE+1+i];
        }
#ifdef DEBUG
        printf("INFO_%d: Message in comm message structure %u: \n",getpid(),nvmsg->mesg_lng);
        for(int i = 0 ; i < nvmsg->mesg_lng; i++){
            printf("_%u",nvmsg->msg[i]);
        }
        printf("\n");
#endif
    }else{
        printf("ERRO_%d:Recived message has raised error %x",getpid(),error);
    }
    return nvmsg;
}
void destroy_msg(struct comm_message *tokill){
    //printf("INFO_%d:Destroying commm message\n");
    if(tokill!= NULL){
        free(tokill->msg);
        free(tokill);
    }
}

