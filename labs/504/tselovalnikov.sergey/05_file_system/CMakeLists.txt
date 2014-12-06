cmake_minimum_required(VERSION 2.8.4)
project(05_file_system)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    src/main.cpp
    src/FileSystem.h
    src/Context.h
    src/Context.cpp
    src/utils/ProcessUtils.h
    src/utils/ProcessUtils.cpp
    src/utils/StringUtils.h
    src/utils/FileUtils.h

    src/INode.h
    src/INode.cpp
    src/utils/BinReader.h
    src/utils/BinWriter.h



    src/cmds/test.cpp



    )
add_executable(05_file_system ${SOURCE_FILES})
