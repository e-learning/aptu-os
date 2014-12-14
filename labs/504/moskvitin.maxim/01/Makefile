MAIN_OUT=./shell
MAIN_SRC=command_execute.c command_parsing.c Vector.c ls.c main.c
PS_OUT=./ps.so
PS_SRC=ps.c
build: clean ps
	gcc -L. -Wall -Werror -o $(MAIN_OUT) $(MAIN_SRC) -l:$(PS_OUT)
clean:
	rm -f $(MAIN_OUT) $(PS_OUT)
ps:
	gcc -shared -Wall -Werror -fpic -o $(PS_OUT) $(PS_SRC)
