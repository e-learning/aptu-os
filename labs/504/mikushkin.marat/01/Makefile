all:
	#gcc -c -Wall -Werror -fpic src/ps.c
	#gcc -shared -o libps.so ps.o
	#gcc -L./ -Wall -o shell src/main.c -lps -std=c99
	#export LD_LIBRARY_PATH=./:$LD_LIBRARY_PATH
	gcc -Wall src/main.c src/dir.c src/execute.c src/parse_input.c src/pipes.c -o shell -std=c99