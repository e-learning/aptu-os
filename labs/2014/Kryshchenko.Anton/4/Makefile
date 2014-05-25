CC=g++
CCFLAGS = -Wall -pedantic
APPNAME = allocator
DEPS=memoryallocator.h memoryallocator.cpp main.cpp

%.o: %.cpp $(DEPS)
	$(CC) $(CCFLAGS) -c $< -o $@ 

all:
	$(CC) $(CCFLAGS) memoryallocator.cpp main.cpp -o allocator

clean_temp:
	rm -f *.o

clean: clean_temp
	rm -f $(APP)
