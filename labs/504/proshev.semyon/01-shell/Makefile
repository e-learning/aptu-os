COMPILER_FLAGS=-Werror -Wall
COMPILER=g++

shell: main.o libps.so
	$(COMPILER) main.o -L. -lps -o shell

main.o: main.cpp
	$(COMPILER) $(COMPILER_FLAGS) -c main.cpp

ps.o: ps.cpp
	$(COMPILER) $(COMPILER_FLAGS) -fPIC -c ps.cpp

libps.so: ps.o
	$(COMPILER) -shared -o libps.so ps.o

.PHONY: clean

clean:
	rm -f main.o ps.o libps.so shell