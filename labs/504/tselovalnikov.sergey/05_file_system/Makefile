CXX = g++
CXXFLAGS = -std=c++11 -Werror
APPS=init format import export ls copy move rm mkdir

all:
	${CXX} ${CXXFLAGS} src/*.cpp src/utils/*.cpp src/cmds/init.cpp -o init
	${CXX} ${CXXFLAGS} src/*.cpp src/utils/*.cpp src/cmds/format.cpp -o format
	${CXX} ${CXXFLAGS} src/*.cpp src/utils/*.cpp src/cmds/import.cpp -o import
	${CXX} ${CXXFLAGS} src/*.cpp src/utils/*.cpp src/cmds/export.cpp -o export
	${CXX} ${CXXFLAGS} src/*.cpp src/utils/*.cpp src/cmds/ls.cpp -o ls
	${CXX} ${CXXFLAGS} src/*.cpp src/utils/*.cpp src/cmds/copy.cpp -o copy
	${CXX} ${CXXFLAGS} src/*.cpp src/utils/*.cpp src/cmds/move.cpp -o move
	${CXX} ${CXXFLAGS} src/*.cpp src/utils/*.cpp src/cmds/rm.cpp -o rm
	${CXX} ${CXXFLAGS} src/*.cpp src/utils/*.cpp src/cmds/mkdir.cpp -o mkdir
	    

clean:
	rm init format import export ls copy move rm mkdir	

