cmake_minimum_required(VERSION 2.8.4)
project(shell)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

set(SOURCE_FILES main.c sh.c commands.c)
add_executable(shell ${SOURCE_FILES})