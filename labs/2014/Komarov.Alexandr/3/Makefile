FLAGS = -std=c++11 -Wall

all: init format import export ls copy move rm mkdir

init: filesystem.o src/init.cpp
	g++ $(FLAGS) filesystem.o src/init.cpp -o init

format: filesystem.o src/format.cpp 
	g++ $(FLAGS) filesystem.o src/format.cpp -o format

import: filesystem.o src/import.cpp
	g++ $(FLAGS) filesystem.o src/import.cpp -o import

export: filesystem.o src/export.cpp
	g++ $(FLAGS) filesystem.o src/export.cpp -o export

ls: filesystem.o src/ls.cpp
	g++ $(FLAGS) filesystem.o src/ls.cpp -o ls

copy: filesystem.o src/copy.cpp
	g++ $(FLAGS) filesystem.o src/copy.cpp -o copy

move: filesystem.o src/move.cpp
	g++ $(FLAGS) filesystem.o src/move.cpp -o move

rm: filesystem.o src/rm.cpp
	g++ $(FLAGS) filesystem.o src/rm.cpp -o rm

mkdir: filesystem.o src/mkdir.cpp
	g++ $(FLAGS) filesystem.o src/mkdir.cpp -o mkdir

filesystem.o: src/FS/filesystem.cpp
	g++ $(FLAGS) -c src/FS/filesystem.cpp

clean:
	rm -rf *.o
	rm -rf init
	rm -rf format
	rm -rf import
	rm -rf export
	rm -rf ls
	rm -rf copy
	rm -rf move
	rm -rf rm
	rm -rf mkdir
	rm -rf filesystem.o

.PHONY: clean

