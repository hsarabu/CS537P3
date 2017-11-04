cmake_minimum_required(VERSION 3.8)
project(CS537P3)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES shm_client_frame.c shm_server_frame.c)
add_executable(CS537P3 ${SOURCE_FILES})