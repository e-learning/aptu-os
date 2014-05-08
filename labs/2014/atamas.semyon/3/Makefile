CXX = g++
CXXFLAGS = -Wall -std=c++11 -O2

SOURCES = $(wildcard *.cpp)

all: init format import export ls copy move rm mkdir

init: fs.o structs.o init.cpp
	$(CXX) $(CXXFLAGS) fs.o structs.o init.cpp -o init

format: fs.o structs.o format.cpp 
	$(CXX) $(CXXFLAGS) fs.o structs.o format.cpp -o format

import: fs.o structs.o import.cpp 
	$(CXX) $(CXXFLAGS) fs.o structs.o import.cpp -o import

export: fs.o structs.o export.cpp 
	$(CXX) $(CXXFLAGS) fs.o structs.o export.cpp -o export

ls: fs.o structs.o ls.cpp 
	$(CXX) $(CXXFLAGS) fs.o structs.o ls.cpp -o ls

copy: fs.o structs.o copy.cpp 
	$(CXX) $(CXXFLAGS) fs.o structs.o copy.cpp -o copy

move: fs.o structs.o move.cpp 
	$(CXX) $(CXXFLAGS) fs.o structs.o move.cpp -o move

rm: fs.o structs.o rm.cpp 
	$(CXX) $(CXXFLAGS) fs.o structs.o rm.cpp -o rm

mkdir: fs.o structs.o mkdir.cpp 
	$(CXX) $(CXXFLAGS) fs.o structs.o mkdir.cpp -o mkdir


fs.o: fs.cpp
	$(CXX) $(CXXFLAGS) -c fs.cpp

structs.o: structs.cpp
	$(CXX) $(CXXFLAGS) -c structs.cpp



