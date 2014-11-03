FLAGS=-Wall -Werror -O2 -std=c++0x
COMMANDS=init format import export ls copy move rm mkdir
all: meta_fs.o $(COMMANDS)
meta_fs.o: meta_fs.cpp meta_fs.h
	g++ $(FLAGS) -c meta_fs.cpp -o meta_fs.o
$(COMMANDS): meta_fs.o meta_fs.h
	g++ $(FLAGS) $@.cpp meta_fs.o -o $@
clean:
	rm -f *.o
	rm -f $(COMMANDS)
