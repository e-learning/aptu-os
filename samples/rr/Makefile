exe: main.o list.o task.o
	gcc main.o list.o task.o -o exe

main.o: main.cpp list.h task.h
	gcc -c main.cpp 

list.o: list.cpp list.h
	gcc -c list.cpp

task.o: task.cpp task.h
	gcc -c task.cpp

clean:
	-rm exe
	-rm *.o

