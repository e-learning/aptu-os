shell: main.o ls.o ps.o kill.o pwd.o
	g++ main.o ls.o pwd.o ps.o kill.o -g -o shell
main.o: main.cpp  command_handler.h
	g++ -c main.cpp -g -o main.o

kill.o: kill.cpp command_handler.h
	g++ -c kill.cpp -g -o kill.o
ps.o: ps.cpp command_handler.h
	g++ -c ps.cpp -g -o ps.o
pwd.o: pwd.cpp command_handler.h
	g++ -c pwd.cpp -g -o pwd.o

ls.o: ls.cpp command_handler.h
	g++ -c ls.cpp -g -o ls.o


clean:
	rm -f *.o shell