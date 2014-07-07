ver2:ps main2.o
	g++ main2.o -ldl
	./a.out

main2.o:main2.cpp
	gcc -c main2.cpp -o main2.o


ver1:ps main1.o
	g++ main1.o -L. -lps -Wl,-rpath,. 
	./a.out

main1.o:main1.cpp
	gcc -c main1.cpp -o main1.o


ps:ps.cpp
	gcc -o ps.o -c ps.cpp
	g++ -shared -o libps.so ps.o

clean:
	rm *.o
