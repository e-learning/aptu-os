CC=g++
CFLAGS=-std=gnu++11 -c -Wall
LDFLAGS=-std=gnu++11 
SOURCES=OS_lab_3.cpp MCB.h ToyAllocator.h ToyAllocator.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=allocator

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm *.o $(EXECUTABLE)