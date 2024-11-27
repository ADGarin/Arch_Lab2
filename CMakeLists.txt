cmake_minimum_required(VERSION 3.27)
project(Arch_Lab_2 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)


add_executable(server server.c)
add_executable(client client.c)
