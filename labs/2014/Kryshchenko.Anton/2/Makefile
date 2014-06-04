CC=g++
CCFLAGS = -Wall
APPNAME = scheduler
DEPS=task.h main.cpp

%.o: %.cpp $(DEPS)
	$(CC) $(CCFLAGS) -c $< -o $@ 

all:
	$(CC) $(CCFLAGS) main.cpp -o scheduler

clean_temp:
	rm -f *.o

clean: clean_temp
	rm -f $(APP)
