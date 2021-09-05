cmake_minimum_required(VERSION 3.20)
project(kashell C)

if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
    message( FATAL_ERROR "In-source builds not allowed! Create a build directory and run CMake from there." )
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(kashell kashel.c cmd.c cmd.h jobctl.c jobctl.h)
add_executable(ppgid syscall-samples/ppgid.c)
add_executable(execvp syscall-samples/execvp.c)
add_executable(pipe syscall-samples/pipe.c)
add_executable(pipeout syscall-samples/pipeout.c)
