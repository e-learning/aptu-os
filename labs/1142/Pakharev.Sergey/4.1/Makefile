CC=g++
CFLAGS=-c -Wall

all: init format import export mkdir info
init: init.o
	$(CC) init.o -o init
	rm *.o    
init.o: init.cpp
	$(CC) $(CFLAGS) init.cpp
format: format.o
	$(CC) format.o -o format
	rm *.o    
format.o: format.cpp
	$(CC) $(CFLAGS) format.cpp
import: import.o
	$(CC) import.o -o import
	rm *.o    
import.o: import.cpp
	$(CC) $(CFLAGS) import.cpp
export: export.o
	$(CC) export.o -o export
	rm *.o    
export.o: export.cpp
	$(CC) $(CFLAGS) export.cpp
mkdir: mkdir.o
	$(CC) mkdir.o -o mkdir
	rm *.o    
mkdir.o: mkdir.cpp
	$(CC) $(CFLAGS) mkdir.cpp
info: info.o
	$(CC) info.o -o info
	rm *.o    
info.o: info.cpp
	$(CC) $(CFLAGS) info.cpp
