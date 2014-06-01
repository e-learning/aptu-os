CC=g++
CFLAGS=-Wall -std=c++11
DEPS=fs.h data.h string_utils.h
APPS=init format import export ls copy move rm mkdir

all: fs.o $(APPS)
	echo lol

fs.o: fs.cpp $(DEPS)
	$(CC) -c fs.cpp -o fs.o $(CFLAGS)

$(APPS): fs.o $(DEPS)
	$(CC) $@.cpp fs.o $(CFLAGS) -o $@

clean_temp:
	rm -f *.o

clean: clean_temp
	rm -f $(APPS)
	rm -f 1*
	rm -f 2*
	rm -f 3*
	rm -f 4*
	rm -f 5*
	rm -f 6*
	rm -f 7*
	rm -f 8*
	rm -f 9*
	rm -f 0

