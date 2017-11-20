//
// Created by pookie on 17/11/17.
//

#ifndef CPROJ_MESSAGE_DEFINES_H_H
#define CPROJ_MESSAGE_DEFINES_H_H

#include <stdlib.h>
#include <stdlib.h>
#define BLOCK_SYNC  ((u_int8_t)0x55)

#define OFFSET_SYNC ((size_t)0)
#define OFFSET_LNGT ((size_t)1)
#define OFFSET_TYPE ((size_t)2)
#define OFFSET_CRS(mssl) ((size_t)(mssl-1))

#define UNDEFINED_TYPE 0x09
#define MIN_RECV_LNG 6
#define CONTROLBLOCKSIZE 4
typedef enum MESSAGE_TYPE {
    CONNECT =(u_int8_t) 0x01,
    OK_NOK = 0x02,
    NEW_MOVE = 0x03,
    END_MSG = 0x04,
    NEXT_TURN = 0x05,
    STATUS_1 = 0x06,
    STATIS_2 = 0x07,
    CONTROL = 0x08,
    PLAYER_OK =(u_int8_t) 0x10,

};
typedef enum MSG_ERROR {
    NO_ERROR = 0, MISSING_SYNC = -1, UNKNOWN_TYPE = -2, WRONG_LENGTH = -3, CRC_ERROR = -4
};
#endif //CPROJ_MESSAGE_DEFINES_H_H
