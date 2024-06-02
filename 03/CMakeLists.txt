cmake_minimum_required(VERSION 3.5)
project(Blatt03 C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

# Find the necessary libraries
find_library(IPC_LIBRARY NAMES rt)
if(NOT IPC_LIBRARY)
    message(FATAL_ERROR "System V IPC library not found")
endif()

# Include directories for rpc headers
include_directories(/usr/include/tirpc)

add_executable(aufgabe_01 src/01.c)
target_link_libraries(aufgabe_01 ${IPC_LIBRARY})

add_executable(aufgabe_02_client src/math_clnt.c 
                                 src/math_xdr.c 
                                 src/math_client.c)
target_link_libraries(aufgabe_02_client nsl tirpc)

add_executable(aufgabe_02_server src/math_svc.c 
                                 src/math_xdr.c 
                                 src/math_server.c)
target_link_libraries(aufgabe_02_server nsl tirpc)