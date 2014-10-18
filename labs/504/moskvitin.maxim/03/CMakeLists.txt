cmake_minimum_required(VERSION 2.8.4)
project(alloc)

set(SOURCE_FILES main.c alloc.c)
add_executable(alloc ${SOURCE_FILES})