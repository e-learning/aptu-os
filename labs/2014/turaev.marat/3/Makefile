CC=clang++
CFLAGS=-Wall -std=c++11
DEPS=file_system.h structs.h
APPS=init format import export ls copy move rm mkdir
TEST_DIR=/tmp
FS_DIR=$TEST_DIR/fs

all: file_system.o $(APPS)

file_system.o: file_system.cpp $(DEPS)
	@$(CC) -c file_system.cpp -o file_system.o $(CFLAGS)

$(APPS): file_system.o $(DEPS)
	@echo "$@"
	@$(CC) $@.cpp file_system.o $(CFLAGS) -o $@

test:
	@echo "content" > tests/content
	@sh ./tests/setup.sh
	@./tests/test.sh tests/content /tmp/fs

clean: 
	@rm -f *.o
	@rm -f $(APPS)
