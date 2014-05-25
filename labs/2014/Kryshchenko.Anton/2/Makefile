CC=g++
CCFLAGS = -std=c++0x 
APPNAME = shell
DEPS=task.h task.cpp sheduler.h sheduler.cpp main.cpp

%.o: %.cpp $(DEPS)
	$(CC) $(CCFLAGS) -c $< -o $@ 

all:
	$(CC) $(CCFLAGS) task.cpp sheduler.cpp main.cpp -o scheduler

clean_temp:
	rm -f *.o

clean: clean_temp
	rm -f $(APP)

