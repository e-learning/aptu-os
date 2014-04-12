#CXXFLAGS  += -O2
CXXFLAGS  += -std=c++0x
CXXFLAGS  += -pg -D_DEBUG -g -Wall

.PHONY: all clean

all: allocator

allocator: main.cpp
	g++ main.cpp ${CXXFLAGS} -o allocator

clean:
	rm -rf allocator *.o