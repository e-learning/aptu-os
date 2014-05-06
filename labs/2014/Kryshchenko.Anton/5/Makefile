CC=g++
CCFLAGS = -Wall  -std=c++0x 
APPNAME = memory
DEPS=main.cpp

%.o: %.cpp $(DEPS)
	$(CC) $(CCFLAGS) -c $< -o $@ 

all:
	$(CC) $(CCFLAGS) main.cpp -o memory

clean_temp:
	rm -f *.o

clean: clean_temp
	rm -f $(APP)
