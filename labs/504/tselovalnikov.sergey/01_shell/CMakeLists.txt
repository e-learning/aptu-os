cmake_minimum_required(VERSION 2.8.4)
project(shell)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=gnu++0x)
endif()

set(SOURCE_FILES
    main.cpp
    src/Terminal.h
    src/Terminal.cpp
    src/cmds/Cmd.h
    src/cmds/PwdCmd.h
    src/cmds/PwdCmd.cpp
    src/cmds/CmdFactory.h
    src/cmds/CmdFactory.cpp
    src/cmds/ExecCmd.h
    src/cmds/ExecCmd.cpp
    src/cmds/ExitCmd.h
    src/cmds/ExitCmd.cpp
    src/cmds/HasArgsCmd.h
    src/cmds/HasArgsCmd.cpp
    src/cmds/LsCmd.h
    src/cmds/LsCmd.cpp
    src/cmds/KillCmd.h
    src/cmds/KillCmd.cpp
    src/cmds/EmptyCmd.h
    src/cmds/EmptyCmd.cpp
    src/cmds/PsCmd.h
    src/cmds/PsCmd.cpp
)
set(SOURCE_LIB
    src/utils/StringUtils.h
    src/utils/FileUtils.h
    src/utils/ProcessUtils.h
    src/utils/ProcessUtils.cpp
)

add_library(utils STATIC ${SOURCE_LIB})
add_executable(shell ${SOURCE_FILES})

target_link_libraries(shell utils)
