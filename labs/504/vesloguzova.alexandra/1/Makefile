CPP_Flags = -Werror -Wall
all: bin_shell
ps.o: ps.c common.h
	gcc $(CPP_Flags) -fPIC -c ps.c
libps.so: ps.o common.h
	gcc -shared -o libps.so ps.o
main.o: main.c
	gcc $(CPP_Flags) -c main.c
bin_shell: main.o libps.so
	gcc ${CPP_Flags} main.o -L. -lps -o bin_shell
clean:
	rm -f  *.o *.so bin_shell
.PHONY: clean
