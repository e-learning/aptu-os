CFLAGS= -c -std=c++0x -Wall   

all: init format import export ls mkdir copy move rm 

init: fsinit.o init.o libfs.a
	g++ -o init fsinit.o init.o libfs.a

fsinit.o: fsinit.cc
	g++ $(CFLAGS) fsinit.cc

init.o: init.cc
	g++ $(CFLAGS) init.cc

libfs.a: bitmap.o blockreader.o dirmanager.o blocklistiterator.o blockwriter.o fscontroller.o 
	ar rvs libfs.a bitmap.o blockreader.o dirmanager.o blocklistiterator.o blockwriter.o fscontroller.o 

bitmap.o: bitmap.cc
	g++ $(CFLAGS) bitmap.cc

blockreader.o: blockreader.cc
	g++ $(CFLAGS) blockreader.cc

dirmanager.o: dirmanager.cc
	g++ $(CFLAGS) dirmanager.cc

blocklistiterator.o: blocklistiterator.cc
	g++ $(CFLAGS) blocklistiterator.cc

blockwriter.o: blockwriter.cc
	g++ $(CFLAGS) blockwriter.cc

fscontroller.o: fscontroller.cc
	g++ $(CFLAGS) fscontroller.cc

import: import.o libfs.a
	g++ -o import import.o libfs.a

import.o: import.cc
	g++ $(CFLAGS) import.cc

export: export.o libfs.a
	g++ -o export export.o libfs.a

export.o: export.cc
	g++ $(CFLAGS) export.cc

ls: ls.o libfs.a
	g++ -o ls ls.o libfs.a

ls.o: ls.cc
	g++ $(CFLAGS) ls.cc

rm: rm.o libfs.a
	g++ -o rm rm.o libfs.a

rm.o: rm.cc
	g++ $(CFLAGS) rm.cc

mkdir: mkdir.o libfs.a
	g++ -o mkdir mkdir.o libfs.a 

mkdir.o: mkdir.cc
	g++ $(CFLAGS) mkdir.cc

format: format.o libfs.a 
	g++ -o format format.o libfs.a

format.o: format.cc
	g++ $(CFLAGS) format.cc

copy: copy.o libfs.a
	g++ -o copy copy.o libfs.a

copy.o: copy.cc
	g++ $(CFLAGS) copy.cc

move: move.o libfs.a
	g++ -o move move.o libfs.a

move.o: move.cc
	g++ $(CFLAGS) move.cc

clean:
	rm -rf *.o init format rm import export mkdir copy ls move libfs.a

