all:
	g++ -c -Wall -Werror -fpic src/utils.cpp
	g++ -shared -o libutils.so utils.o
	g++ -L./ -Wall -o shell src/main.cpp -lutils -std=c++11
	#export LD_LIBRARY_PATH=./:$LD_LIBRARY_PATH