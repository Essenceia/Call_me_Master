//
// Created by pookie on 17/11/17.
//

#ifndef CPROJ_MESSAGE_DEFINES_H_H
#define CPROJ_MESSAGE_DEFINES_H_H

#include <stdlib.h>
#define BLOCK_SYNC  ((u_int8_t)0x55)

#define OFFSET_SYNC 0
#define OFFSET_LNGT 1
#define OFFSET_TYPE 2
#define OFFSET_CRS(mssl) ((mssl)-1)
#define CONTROLBLOCKSIZE 4
typedef enum {
    CONNECT =(u_int8_t) 0x01,
    OK_NOK = 0x02,
    NEW_MOVE = 0x03,
    END_MSG = 0x04,
    NEXT_TURN = 0x05,
    STATUS_1 = 0x06,
    STATIS_2 = 0x07,
    CONTROL = 0x08,
    SKIP_TURN = 0x09,//todo - change
    PLAYER_OK =(u_int8_t) 0x10,
    PING = 0x11,

}MESSAGE_TYPE ;
typedef enum {
    NO_ERROR = 0, MISSING_SYNC = -1, UNKNOWN_TYPE = -2, WRONG_LENGTH = -3, CRC_ERROR = -4
}MSG_ERROR;
#endif //CPROJ_MESSAGE_DEFINES_H_H
