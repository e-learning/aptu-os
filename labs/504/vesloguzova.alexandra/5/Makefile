CXX = g++
CXXFLAGS = -Wall -std=c++11 -O2

SOURCES = $(wildcard *.cpp)

all: init format import export ls copy move rm mkdir

init: file_system.o util.o init.cpp
	$(CXX) $(CXXFLAGS) file_system.o util.o init.cpp -o init

format: file_system.o util.o format.cpp
	$(CXX) $(CXXFLAGS) file_system.o util.o format.cpp -o format

import: file_system.o util.o import.cpp
	$(CXX) $(CXXFLAGS) file_system.o util.o import.cpp -o import

export: file_system.o util.o export.cpp
	$(CXX) $(CXXFLAGS) file_system.o util.o export.cpp -o export

ls: file_system.o util.o ls.cpp
	$(CXX) $(CXXFLAGS) file_system.o util.o ls.cpp -o ls

copy: file_system.o util.o copy.cpp
	$(CXX) $(CXXFLAGS) file_system.o util.o copy.cpp -o copy

move: file_system.o util.o move.cpp
	$(CXX) $(CXXFLAGS) file_system.o util.o move.cpp -o move

rm: file_system.o util.o rm.cpp
	$(CXX) $(CXXFLAGS) file_system.o util.o rm.cpp -o rm

mkdir: file_system.o util.o mkdir.cpp
	$(CXX) $(CXXFLAGS) file_system.o util.o mkdir.cpp -o mkdir


file_system.o: file_system.cpp
	$(CXX) $(CXXFLAGS) -c file_system.cpp

util.o: util.cpp
	$(CXX) $(CXXFLAGS) -c util.cpp

.PHONY: clean

clean:
	rm -f *.o
	rm init
	rm format
	rm import
	rm export
	rm ls
	rm copy
	rm move
	rm rm
	rm mkdir


