all:
	g++ -c -Wall -Werror -fpic src/ps.cpp
	g++ -shared -o libps.so ps.o
	g++ -L./ -Wall -o shell src/main.cpp src/utils.cpp -lps -std=c++11
	#export LD_LIBRARY_PATH=./:$LD_LIBRARY_PATH