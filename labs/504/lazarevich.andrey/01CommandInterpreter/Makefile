FLAGS = -Wall -Werror
SRC = kill.c ls.c main.c pwd.c try_run.c
OBJ = kill.o ls.o main.o pwd.o try_run.o

all: my_shell

$(OBJ): $(SRC) common.h
	gcc $(FLAGS) -c $(SRC)

ps.o: ps.c common.h
	gcc $(FLAGS) -fPIC -c ps.c

libps.so: ps.o common.h
	gcc -shared -o libps.so ps.o

my_shell: $(OBJ) libps.so
	gcc $(OBJ) -L. -lps -o my_shell

clean:
	rm -f my_shell $(OBJ) ps.o libps.so

