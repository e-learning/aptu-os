CC=g++
CCFLAGS = -Wall -pedantic
APPNAME = shell
DEPS=interpreter.h interpreter.cpp main.cpp

%.o: %.cpp $(DEPS)
	$(CC) $(CCFLAGS) -c $< -o $@ 

all:
	$(CC) $(CCFLAGS) interpreter.cpp main.cpp -o shell

clean_temp:
	rm -f *.o

clean: clean_temp
	rm -f $(APP)
