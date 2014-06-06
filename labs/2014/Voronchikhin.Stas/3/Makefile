FLAGS = -std=c++11 -Wall

OBJECTS = filesystem.o  block_pack.o block.o  directory_iterator.o


all: init format import export ls copy move rm mkdir

init: $(OBJECTS) src/utils/init.cpp
	g++ $(FLAGS) $(OBJECTS) src/utils/init.cpp -o init

format: $(OBJECTS) src/utils/format.cpp 
	g++ $(FLAGS) $(OBJECTS) src/utils/format.cpp -o format

import: $(OBJECTS) src/utils/import.cpp
	g++ $(FLAGS) $(OBJECTS) src/utils/import.cpp -o import

export: $(OBJECTS) src/utils/export.cpp
	g++ $(FLAGS) $(OBJECTS) src/utils/export.cpp -o export

ls: $(OBJECTS) src/utils/ls.cpp
	g++ $(FLAGS) $(OBJECTS) src/utils/ls.cpp -o ls

copy: $(OBJECTS) src/utils/copy.cpp
	g++ $(FLAGS) $(OBJECTS) src/utils/copy.cpp -o copy

mkdir: $(OBJECTS) src/utils/mkdir.cpp
	g++ $(FLAGS) $(OBJECTS) src/utils/mkdir.cpp -o mkdir

rm: $(OBJECTS) src/utils/rm.cpp
	g++ $(FLAGS) $(OBJECTS) src/utils/rm.cpp -o rm

move: $(OBJECTS) src/utils/move.cpp
	g++ $(FLAGS) $(OBJECTS) src/utils/move.cpp -o move


directory_iterator.o:  src/main/directory_iterator.cpp
	g++ $(FLAGS) -c src/main/directory_iterator.cpp

filesystem.o:  src/main/filesystem.cpp
	g++ $(FLAGS) -c src/main/filesystem.cpp
block.o:  src/main/block.cpp
	g++ $(FLAGS) -c src/main/block.cpp

block_pack.o:   src/main/block_pack.cpp
	g++ $(FLAGS) -c src/main/block_pack.cpp



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



