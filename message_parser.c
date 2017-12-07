//
// Created by pookie on 17/11/17.
//
#include "message_parser.h"
#include "message_helper.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <memory.h>

#define DEBUG

comm_message *parse_recv_msg(u_int8_t *recvmsg, u_int8_t recvlngth) {
#ifdef DEBUG
    printf("INFO_%d: Recived message length %u: \n", pthread_self(), recvlngth);
    for (int i = 0; i < recvlngth; i++) {
        printf("_%c(%u)", recvmsg[i], recvmsg[i]);
    }
    printf("\n");
#endif
    comm_message *nvmsg = NULL;
    u_int8_t error = check_error(recvmsg, recvlngth);
    if (error >= 0) {
        nvmsg = (comm_message *) malloc(sizeof(comm_message));
        nvmsg->type = (MESSAGE_TYPE) recvmsg[OFFSET_TYPE];
        nvmsg->mesg_lng = (u_int8_t) recvmsg[OFFSET_LNGT];
        if (nvmsg->mesg_lng)nvmsg->msg = (u_int8_t *) malloc(sizeof(u_int8_t) * nvmsg->mesg_lng);
        else nvmsg->msg = NULL;
            memcpy(nvmsg->msg,recvmsg+OFFSET_TYPE + 1,nvmsg->mesg_lng);

#ifdef DEBUG
        printf("INFO_%d: Message in comm message structure , length %u: and type %x \n", pthread_self(), nvmsg->mesg_lng,nvmsg->type);
        for (int i = 0; i < nvmsg->mesg_lng; i++) {
            printf("_%u", nvmsg->msg[i]);
        }
        printf("\n");
#endif
    } else {
        printf("ERRO_%d:Recived message has raised error %x", pthread_self(), error);
    }
    return nvmsg;
}

void destroy_msg(comm_message *tokill) {
    //printf("INFO_%d:Destroying commm message\n");
    if (tokill != NULL) {
        //if (tokill->msg != NULL)free(tokill->msg);
        free(tokill);
        tokill = NULL;
    }
}

