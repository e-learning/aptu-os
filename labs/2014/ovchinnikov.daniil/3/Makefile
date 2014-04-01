CC=g++
CFLAGS=-Wall -std=c++11
DEPS=

%.o: %.cpp $(DEPS)
	$(CC) -c $< -o $@ $(CFLAGS)

init: init.o
	$(CC) $< -o init

clean_temp:
	rm -f *.o

clean: clean_temp
	rm -f $(APP)
	rm 1*
	rm 2*
	rm 3*
	rm 4*
	rm 5*
	rm 6*
	rm 7*
	rm 8*
	rm 9*
	rm 0

