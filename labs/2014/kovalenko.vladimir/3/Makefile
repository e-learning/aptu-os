CC=g++
CFLAGS=-Wall -std=c++11
DEPS=filesystem.h entries.h
APPS=init format import export ls copy move rm mkdir

all: filesystem.o $(APPS)


filesystem.o: filesystem.cpp $(DEPS)
	$(CC) -c filesystem.cpp -o filesystem.o $(CFLAGS)

$(APPS): filesystem.o $(DEPS)
	$(CC) $@.cpp filesystem.o $(CFLAGS) -o $@

cleantemp:
	rm -f *.o

clean: cleantemp
	rm -f $(APPS)

