//
// Created by pookie on 17/11/17.
//
#include "message_helper.h"
#include "message_defines.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#define DEBUG

char check_valide_legth(u_int8_t tmsglng, u_int8_t msgl){
    //verify that the total message length and app message
    //length are of appropriate size
    char res;
    res = ((tmsglng == (msgl + CONTROLBLOCKSIZE))? '1' : '0' );
    printf("INFO_:Message length %o expected length %o, result is %u\n",tmsglng,(msgl + CONTROLBLOCKSIZE),res);

    return res;
}
char check_type_valide(char type){
    if((type >=CONNECT)&&(type <=PLAYER_OK)){
        return 1;
    }else{
        printf("ERRO_:Type of message is invalide errro, got type %x \n",type);
        return 0;
    }
}
//product the crc we would be expecting here
u_int8_t get_crc(u_int8_t *appmsg,size_t alng){
    u_int8_t accumulater=0;
    printf("INFO_%d:Calculating CRC adding up length %u: \n",getpid(),alng);
    for(u_int8_t i = 0 ; i < alng ; i++){
        printf("0x%x ",appmsg[i]);
        accumulater +=appmsg[i];

    }
    printf("\nINFO_: CRC value : 0x%x\n",accumulater);
    return ((u_int8_t)(accumulater&0xFF));
}
u_int8_t check_error(u_int8_t *recvmsg,u_int8_t recvlng){
    enum MSG_ERROR error = NO_ERROR;
    if(recvlng >= MIN_RECV_LNG) {
        if (recvmsg[OFFSET_SYNC] == BLOCK_SYNC) {
            if (check_type_valide(recvmsg[OFFSET_TYPE])) {
                if(check_valide_legth(recvlng,recvmsg[ OFFSET_LNGT ] )=='1' ){
                    puts("INFO_:No error in lenght\n");
                    if(get_crc(recvmsg,recvlng-1)==recvmsg[OFFSET_CRS(recvlng)]){
                        error = NO_ERROR;
                        puts("INFO_:NO ERROR");

                    }else
                    {
                        error = CRC_ERROR;
                        puts("INFO_:CRC ERROR");
                    }
                }else {
                    error = WRONG_LENGTH;
                    puts("INFO_:WRONG LENGTH");
                }

            } else
            {
                error = UNKNOWN_TYPE;
                puts("INFO_:UNKNOWN_TYPE");
            }
        } else{
            puts("INFO_:MISSING_SYNC");
            error = MISSING_SYNC;
        }
    }else{
        puts("INFO_:WRONG_LENGTH");
        error = WRONG_LENGTH;
    }
    return error;
}
u_int8_t *prepare_message(comm_message *tosend){
#ifdef DEBUG
    printf("INFO_%d:Preparing message of type 0x%x , length %u\n",getpid(),tosend->type,tosend->mesg_lng);
#endif
    u_int8_t *repared=NULL;
    if(tosend->mesg_lng>=0 && check_type_valide(tosend->type)) {
        repared = (u_int8_t *) malloc(sizeof(CONTROLBLOCKSIZE + tosend->mesg_lng));
        repared[OFFSET_SYNC] = BLOCK_SYNC;
        repared[OFFSET_TYPE] = tosend->type;
        repared[OFFSET_LNGT]=tosend->mesg_lng;
        for(int i = 0 ; i<tosend->mesg_lng; i++){
            repared[OFFSET_TYPE+1+i]=tosend->msg[i];
        }
        repared[CONTROLBLOCKSIZE+tosend->mesg_lng-1]=get_crc(repared,(sizeof(repared)/sizeof(u_int8_t)));
    }
#ifdef DEBUG
    printf("INFO_%d:Prepared message with parameters sync %x type %x length %o :\n",getpid(),0x55,tosend->type,tosend->mesg_lng);
    for(int i = 0 ; i < (tosend->mesg_lng+CONTROLBLOCKSIZE); i++){
        printf("_0x%x",repared[i]);
    }
    printf("\n");
#endif
    return repared;

}
int8_t sendof(int socket,comm_message* tosend){
    int8_t retv =0;
    u_int8_t *officle_msg=prepare_message(tosend);

    if(officle_msg!=NULL) {
        if (send(socket, officle_msg, tosend->mesg_lng + CONTROLBLOCKSIZE, 0) == -1) {
            printf("ERRO_:%d Message send failed\n", getpid());
            perror("ERRO_:Sending message failed reason");
            retv = -1;
        }
        free(officle_msg);
    }else{
        printf("ERRO_%d:Our message had problemes in preperation\n",getpid());
    }
    destroy_msg(tosend);
    return retv;
}
/*
void msg_player_ok(u_int8_t playertype, int socket){
    comm_message *nwmsg=(comm_message*)malloc(sizeof(comm_message));
    u_int8_t *msg=(u_int8_t *)malloc(sizeof(u_int8_t)*1);
    msg[0]=((playertype==0x01)?0x01:0x02);//if type == BP
    printf("INFO_%d:Client type %o",getpid(),playertype);
    nwmsg->mesg_lng=2;
    nwmsg->type=PLAYER_OK;
    nwmsg->msg=msg;
    sendof(socket,nwmsg);
}*/
void msg_player_ok(u_int8_t player,int socket){
    comm_message *nwmsg=(comm_message*)malloc(sizeof(comm_message));
    u_int8_t *msg=(u_int8_t *)malloc(sizeof(u_int8_t)*1);
    if(player!=0x01 && player!=0x02){
        printf("ERRO_%d:Ok Nok value is wrong , value got %x\n",getpid(),player);
        player = 0x01;
    }
    msg[0]=player;
    nwmsg->mesg_lng=1;
    nwmsg->type=PLAYER_OK;
    nwmsg->msg=msg;
    sendof(socket,nwmsg);
}
void msg_oknok(u_int8_t oknok, int socket){
    comm_message *nwmsg=(comm_message*)malloc(sizeof(comm_message));
    u_int8_t *msg=(u_int8_t *)malloc(sizeof(u_int8_t)*1);
    msg[0]=oknok;
    nwmsg->mesg_lng=1;
    nwmsg->type=OK_NOK;
    nwmsg->msg=msg;
    sendof(socket,nwmsg);
}

/*
void next_turn(int socket){
    comm_message* tosend = (comm_message*)malloc(sizeof(comm_message));
    tosend->msg=board_prepare_msg();
    tosend->mesg_lng=get_board_msg_size();
    tosend->type=NEXT_TURN;
    sendof(socket,tosend);
}*/
/*void msg_ok_nok(u_int8_t socket,u_int8_t oknok){
    u_int8_t *to_send,buf[1];
    buf[0]=oknok;
    to_send = prepare_message(OK_NOK,buf,1);
    sendof(socket,to_send,1+4);
    free(to_send);
}*/
comm_message* recapsulate_for_player(comm_message *to_recapsulate){
    //remove client number in message
    u_int8_t *new_message = (u_int8_t*)malloc(sizeof(u_int8_t)*to_recapsulate->mesg_lng-1);
    for (u_int8_t i = 0; i < to_recapsulate->mesg_lng-1 ; ++i) {
        new_message[i]=to_recapsulate->msg[i+1];
    }
    free(to_recapsulate->msg);
    to_recapsulate->mesg_lng--;
    to_recapsulate->msg=new_message;
    return to_recapsulate;
}

comm_message* recapsulate_for_controller(comm_message *sendtoclient, u_int8_t client_id){
    u_int8_t *newmsg = (u_int8_t*)malloc(sizeof(u_int8_t)*(sendtoclient->mesg_lng+1));
    newmsg[0]=client_id;
    for(u_int8_t i = 1 ; i < sendtoclient->mesg_lng; i++){
        newmsg[i]=sendtoclient->msg[i-1];
    }
    sendtoclient->mesg_lng --;
    free(sendtoclient->msg);
    sendtoclient->msg=newmsg;
    return sendtoclient;
}