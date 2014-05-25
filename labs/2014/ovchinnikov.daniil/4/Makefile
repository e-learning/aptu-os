CC=g++
CFLAGS=-Wall -std=c++11 -g
DEPS=
APP=allocator

$(APP): main.o $(DEPS)
	$(CC) $^ -o $@ $(CFLAGS)
	
%.o: %.cpp $(DEPS)
	$(CC) -c $^ -o $@ $(CFLAGS)

clean_temp:
	rm -f *.o

clean: clean_temp
	rm -f $(APP)

