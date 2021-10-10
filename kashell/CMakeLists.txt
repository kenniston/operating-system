cmake_minimum_required(VERSION 3.20)
project(kashell C)

if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
    message( FATAL_ERROR "In-source builds not allowed! Create a build directory and run CMake from there." )
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
SET(CMAKE_C_FLAGS -pthread)

add_executable(kashell kashel.c cmd.c cmd.h jobctl.c jobctl.h)
add_executable(ppgid syscall-samples/ppgid.c)
add_executable(execvp syscall-samples/execvp.c)
add_executable(pipe syscall-samples/pipe.c)
add_executable(pipeout syscall-samples/pipeout.c)
add_executable(fpointer syscall-samples/fpointer.c)
add_executable(dup syscall-samples/dup.c)
add_executable(strtok syscall-samples/strtok.c)
add_executable(pthread syscall-samples/pthread.c)

install(FILES "${CMAKE_SOURCE_DIR}/kashell.1" DESTINATION /usr/local/man/man1)
install(FILES ${CMAKE_BINARY_DIR}/kashell DESTINATION /usr/local/bin
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                    GROUP_EXECUTE WORLD_EXECUTE)