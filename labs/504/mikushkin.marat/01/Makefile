all:
	gcc -c -Wall -Werror -fpic ps.c
	gcc -shared -o libps.so ps.o
	gcc -L./ -Wall -o shell main.c -lps -std=c99
	export LD_LIBRARY_PATH=./:$LD_LIBRARY_PATH