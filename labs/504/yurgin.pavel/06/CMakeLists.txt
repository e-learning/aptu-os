cmake_minimum_required(VERSION 2.8.4)
project(sieve_unith)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "/home/noname/svn/aptu-os/labs/504/yurgin.pavel/06")
set(SOURCE_FILES sieve_unith.cpp)
add_executable(sieve_unith ${SOURCE_FILES})

project(sieve_multith)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror -Wl,--no-as-needed -pthread")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "/home/noname/svn/aptu-os/labs/504/yurgin.pavel/06")
set(SOURCE_FILES sieve_multith.cpp)
add_executable(sieve_multith ${SOURCE_FILES})