pthread: obj
	g++ main_pthread.o -L. -pthread -o shell -Wl,-rpath,.
obj: main_pthread.cpp
	g++ -c main_pthread.cpp -o main_pthread.o
clear:
	rm *.o
