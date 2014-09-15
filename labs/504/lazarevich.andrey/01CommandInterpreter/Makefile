FLAGS = -Wall -Werror -g

all: my_shell

my_shell: *.o
	gcc $(FlAGS) *.o -o my_shell

*.o: *.c common.h
	gcc $(FLAGS) -c *.c

clean:
	rm -f my_shell *.o

