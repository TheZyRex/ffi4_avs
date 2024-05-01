cmake_minimum_required(VERSION 3.5)
project(Blatt01 C)

set(CMAKE_C_STANDARD 17)

# Add the executable target
add_executable(aufgabe_01 src/01.c)


# Find the necessary libraries
find_library(IPC_LIBRARY NAMES rt)
if(NOT IPC_LIBRARY)
    message(FATAL_ERROR "System V IPC library not found")
endif()

add_executable(aufgabe_02 src/02.c)
target_link_libraries(aufgabe_02 ${IPC_LIBRARY})