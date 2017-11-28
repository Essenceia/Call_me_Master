//
// Created by pookie on 17/11/17.
//
#include "message_helper.h"
#include "message_defines.h"
#include <stdio.h>
#include <unistd.h>
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
    return (((type >=CONNECT)&&(type <=PLAYER_OK))&&(type!=UNDEFINED_TYPE));
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
u_int8_t *prepare_message(u_int8_t type,u_int8_t *msg,u_int8_t length){
    printf("INFO_%d:Preparing message of type 0x%x , length %u\n",getpid(),type,length);
    u_int8_t *repared;
    if(length>0 && check_type_valide(type)) {
        repared = (u_int8_t *) malloc(sizeof(CONTROLBLOCKSIZE + length));
        repared[OFFSET_SYNC] = BLOCK_SYNC;
        repared[OFFSET_TYPE] = type;
        repared[OFFSET_LNGT]=length;
        for(int i = 0 ; i<length; i++){
            repared[OFFSET_TYPE+1+i]=msg[i];
        }
        repared[CONTROLBLOCKSIZE+length-1]=get_crc(repared,(sizeof(repared)/sizeof(u_int8_t)));
    }
#ifdef DEBUG
    printf("INFO_%d:Prepared message with parameters sync %x type %x length %o :\n",getpid(),0x55,type,length);
    for(int i = 0 ; i < (length+CONTROLBLOCKSIZE); i++){
        printf("_0x%x",repared[i]);
    }
    printf("\n");
#endif
    return repared;

}
u_int8_t* msg_ok(u_int8_t type){

    u_int8_t msg[1];
    msg[0]=((type==0x01)?0x01:0x02);//if type == BP
    printf("INFO_%d:Client type %o",getpid(),type);
    u_int8_t *ret = prepare_message(PLAYER_OK,msg,(sizeof(msg)/sizeof(u_int8_t)));
    return ret;

}
u_int8_t *next_turn(u_int8_t player_id){

}