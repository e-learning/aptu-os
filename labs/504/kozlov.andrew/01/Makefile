CPP=g++-4.8
FLAGS=-std=c++0x -Wall -Werror

objects = common.o ls.o pwd.o kill.o exit.o main.o
common = common.o

all: shell

common.o: common.h common.cpp
	$(CPP) $(FLAGS) -fPIC -c common.cpp

ls.o: $(common) ls.cpp
	$(CPP) $(FLAGS) -c ls.cpp

pwd.o: $(common) pwd.cpp
	$(CPP) $(FLAGS) -c pwd.cpp

ps.o: $(common) ps.cpp
	$(CPP) $(FLAGS) -fPIC -c ps.cpp

libps.so: $(common) ps.o
	$(CPP) -shared -o libps.so $(common) ps.o

kill.o: $(common) kill.cpp
	$(CPP) $(FLAGS) -c kill.cpp

exit.o: $(common) exit.cpp
	$(CPP) $(FLAGS) -c exit.cpp

main.o: $(common) main.cpp
	$(CPP) $(FLAGS) -c main.cpp

shell: $(objects) libps.so
	$(CPP) $(objects) -L. -lps -o shell

.PHONY: clean
clean:
	rm -f shell $(objects) ps.o libps.so
