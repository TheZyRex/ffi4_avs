cmake_minimum_required(VERSION 3.5)
project(Blatt02 C)

set(CMAKE_C_STANDARD 17)

# Find the necessary libraries
find_library(IPC_LIBRARY NAMES rt)
if(NOT IPC_LIBRARY)
    message(FATAL_ERROR "System V IPC library not found")
endif()

add_executable(aufgabe_01 src/01.c)
target_link_libraries(aufgabe_01 ${IPC_LIBRARY})