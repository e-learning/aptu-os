CC = g++
CFLAGS = -Wall -O2 -std=c++11

LIB_SOURCES = $(wildcard src/lib/*.cpp)

def:
	$(CC) $(CFLAGS) $(LIB_SOURCES) src/copy.cpp -o copy
	$(CC) $(CFLAGS) $(LIB_SOURCES) src/export.cpp -o export
	$(CC) $(CFLAGS) $(LIB_SOURCES) src/format.cpp -o format
	$(CC) $(CFLAGS) $(LIB_SOURCES) src/import.cpp -o import
	$(CC) $(CFLAGS) $(LIB_SOURCES) src/init.cpp -o init
	$(CC) $(CFLAGS) $(LIB_SOURCES) src/ls.cpp -o ls
	$(CC) $(CFLAGS) $(LIB_SOURCES) src/mkdir.cpp -o mkdir
	$(CC) $(CFLAGS) $(LIB_SOURCES) src/move.cpp -o move
	$(CC) $(CFLAGS) $(LIB_SOURCES) src/rm.cpp -o rm

clean:
	rm -rf copy
	rm -rf export
	rm -rf format
	rm -rf import
	rm -rf init
	rm -rf ls
	rm -rf mkdir
	rm -rf move
	rm -rf rm

.PHONY: clean
