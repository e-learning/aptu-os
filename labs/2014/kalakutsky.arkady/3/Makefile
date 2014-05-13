#CXXFLAGS  += -O2
CXXFLAGS  += -std=c++0x
CXXFLAGS  += -pg -D_DEBUG -g -Wall
.PHONY: all clean
all: format init ls mkdir rm move copy import export freespace

freespace: free_blocks.cpp fsinfo.o 
	g++ free_blocks.cpp fsinfo.o ${CXXFLAGS} -o freespace

format: format.cpp util.o
	g++ format.cpp util.o ${CXXFLAGS} -o format

init: init.cpp util.o
	g++ init.cpp util.o ${CXXFLAGS} -o init

mkdir: mkdir.cpp util.o fsinfo.o file.o
	g++ mkdir.cpp util.o fsinfo.o file.o ${CXXFLAGS} -o mkdir

rm: rm.cpp util.o file.o fsinfo.o
	g++ rm.cpp util.o file.o fsinfo.o ${CXXFLAGS} -o rm

move: move.cpp util.o file.o fsinfo.o
	g++ move.cpp util.o file.o fsinfo.o ${CXXFLAGS} -o move

copy: copy.cpp util.o file.o fsinfo.o
	g++ copy.cpp util.o file.o fsinfo.o ${CXXFLAGS} -o copy

import: import.cpp util.o file.o fsinfo.o
	g++ import.cpp util.o file.o fsinfo.o ${CXXFLAGS} -o import

export: export.cpp util.o file.o fsinfo.o
	g++ export.cpp util.o file.o fsinfo.o ${CXXFLAGS} -o export

ls: ls.cpp file.o fsinfo.o 
	g++ ls.cpp fsinfo.o file.o util.o ${CXXFLAGS} -o ls

file.o: file.cpp file.h util.o fsinfo.o
	g++ file.cpp   -c ${CXXFLAGS} -o file.o

fsinfo.o: fsinfo.cpp fsinfo.h util.o 
	g++ fsinfo.cpp  -c ${CXXFLAGS} -o fsinfo.o

util.o: util.cpp util.h except.h
	g++ util.cpp -c ${CXXFLAGS} -o util.o

clean:
	rm -f init format ls import export copy move rm mkdir *.o