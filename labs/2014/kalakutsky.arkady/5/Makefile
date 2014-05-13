#CXXFLAGS  += -O2
CXXFLAGS  += -std=c++0x
CXXFLAGS  += -pg -D_DEBUG -g -Wall
.PHONY: all clean
all: memory

memory: main.cpp 
	g++ main.cpp  ${CXXFLAGS} -o memory


clean:
	rm -f memory *.o