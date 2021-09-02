cmake_minimum_required(VERSION 3.20)
project(kashell C)

set(CMAKE_C_STANDARD 11)

add_executable(kashell kashel.c)
add_executable(ppgid syscall-samples/ppgid.c)
