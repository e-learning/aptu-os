CXX = g++
CXXFLAGS = -O2 -g -Wall -fmessage-length=0


all: shell.o
	$(CXX) $(CXXFLAGS) shell.o -o shell
	 
main: shell.cpp 
	$(CXX) $(CXXFLAGS) -c shell.cpp -o shell.o
	
.PHONY: clean

clean:
	rm -f *.o
