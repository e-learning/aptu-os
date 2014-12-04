cmake_minimum_required(VERSION 2.8.4)

project(3)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES  file_system.h file_system.cpp util.h util.cpp)

add_executable( copy copy.cpp ${SOURCE_FILES} )
add_executable( ls ls.cpp ${SOURCE_FILES} )
add_executable( export export.cpp ${SOURCE_FILES} )
add_executable( init init.cpp ${SOURCE_FILES} )
add_executable( import import.cpp ${SOURCE_FILES} )
add_executable( format format.cpp ${SOURCE_FILES} )
add_executable( mkdir mkdir.cpp ${SOURCE_FILES} )
add_executable( move move.cpp ${SOURCE_FILES} )
add_executable( rm rm.cpp ${SOURCE_FILES} )