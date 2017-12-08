//
// Created by pookie on 07/12/17.
//

#include "print_in_color.h"
#include <stdio.h>

void set_process_color(long id){
    switch (id%7){
        case 0: printf("\x1b[31m");
            break;
        case 1:printf("\x1b[32m");
            break;
        case 2:printf("\x1b[33m");
            break;
        case 3:printf("\x1b[34m");
            break;
        case 4:printf("\x1b[35m");
            break;
        case 5: printf("\x1b[36m");
            break;
        default:printf("\x1b[0m");
            break;
    }
}