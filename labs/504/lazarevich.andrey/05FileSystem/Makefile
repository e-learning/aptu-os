FLAGS=-Wall -Werror
TARGETS=init format import export ls copy move rm mkdir
FS=myfs.o
all: 
	g++ $(FLAGS) -std=c++0x -c myfs.cpp myfs.h
	g++ $(FLAGS) -std=c++0x init.cpp $(FS) -o init
	g++ $(FLAGS) -std=c++0x format.cpp $(FS) -o format
	g++ $(FLAGS) -std=c++0x import.cpp $(FS) -o import
	g++ $(FLAGS) -std=c++0x export.cpp $(FS) -o export
	g++ $(FLAGS) -std=c++0x ls.cpp $(FS) -o ls
	g++ $(FLAGS) -std=c++0x copy.cpp $(FS) -o copy
	g++ $(FLAGS) -std=c++0x move.cpp $(FS) -o move
	g++ $(FLAGS) -std=c++0x rm.cpp $(FS) -o rm
	g++ $(FLAGS) -std=c++0x mkdir.cpp $(FS) -o mkdir

.PHONY: clean

clean:
	rm -f *.o *.gch $(TARGETS)
