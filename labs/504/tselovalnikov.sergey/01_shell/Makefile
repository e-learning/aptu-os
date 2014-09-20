all: shell    

shell: utils
	g++ -std=c++11 -L. -Wl,-rpath=. main.cpp src/*.cpp src/cmds/*.cpp  -o shell -lutils

utils:
	g++ -c -std=c++11 -fpic -o utils.o  src/utils/*.cpp  && g++ -shared -o libutils.so utils.o
