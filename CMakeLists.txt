cmake_minimum_required(VERSION 3.8)
project(CProj)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES board_handler.c board_handler.h message_defines.h message_helper.c message_helper.h message_parser.c message_parser.h connection_base_struct.c connection_base_struct.h time_keeper.c time_keeper.h lib_server.c lib_server.h main.c lib_connection.c lib_connection.h)
#add_library(CProj ${SOURCE_FILES})
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
add_executable(NAME main.c board_handler.c board_handler.h message_defines.h message_helper.c message_helper.h message_parser.c message_parser.h connection_base_struct.c connection_base_struct.h time_keeper.c time_keeper.h lib_server.c lib_server.h lib_connection.c lib_connection.h client_registration.c client_registration.h board_handler.c board_handler.h)
#target_link_libraries(NAME ${SOURCE_FILES})
target_link_libraries (NAME ${CMAKE_THREAD_LIBS_INIT})