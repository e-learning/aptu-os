cmake_minimum_required(VERSION 2.8.4)
project(tracert)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(traceroute ${SOURCE_FILES})
