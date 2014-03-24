run: ver2
	./a.out

ver1: main1.o libps.so
	g++ main1.o -L. -lps -Wl,-rpath,. 

ver2: main2.o libps.so
	g++ main2.o -ldl 

main.o: main.cpp 
	gcc -o main1.o -c main1.cpp

main2.o: main2.cpp 
	gcc -o main2.o -c main2.cpp

ps.o: ps.h ps.cpp
	gcc -o ps.o -c ps.cpp

libps.so: ps.o
	gcc -shared -o libps.so ps.o

clear:
	rm *.o

clearAll: clear
	rm a.out
	rm *.so
