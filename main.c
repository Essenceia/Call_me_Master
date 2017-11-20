#include "lib_server.h"
#include "lib_connection.h"
#include <stdio.h>
#include "time_keeper.h"

void warning(){
    //puts("WARN_: 10 second has passed, soon gonna die");
}
int main(){
    int8_t Server_status = init_server();
    printf("INIT server status = %d",Server_status);
    //start server
    Server_status= run_server();
    printf("INFO_:Server status %d",Server_status);


}