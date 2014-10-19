CC=g++
CFLAGS=-std=gnu++11 -c -Wall
LDFLAGS=-std=gnu++11 
SOURCES=OS_lab_4.cpp AddressGetter.h AddressGetter.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=memory

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm *.o $(EXECUTABLE)