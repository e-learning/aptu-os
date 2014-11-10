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

    src/cmds/init.cpp
    src/cmds/format.cpp
    src/cmds/import.cpp
    src/cmds/export.cpp
    src/cmds/ls.cpp
    src/cmds/copy.cpp
    src/cmds/move.cpp
    src/cmds/rm.cpp
    src/cmds/mkdir.cpp


    src/cmds/test.cpp



    )
add_executable(05_file_system ${SOURCE_FILES})
