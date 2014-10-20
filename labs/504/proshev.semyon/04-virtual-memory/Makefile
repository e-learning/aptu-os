CPP_Flags=-Werror -Wall -std=c++0x
CPP=g++

main: main.o
	$(CPP) $(CPP_Flags) main.o -o memory

main.o: main.cpp
	$(CPP) $(CPP_Flags) -c main.cpp

.PHONY: clean

clean:
	rm -f main.o memory