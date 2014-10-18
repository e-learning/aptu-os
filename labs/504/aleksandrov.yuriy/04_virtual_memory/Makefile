FLAGS = -O2 -Wall -Werror 
CC = g++
OUT = memory
DEPS = memory.cpp

all:
	$(CC) $(FLAGS) $(DEPS) -o $(OUT)
debug:
	$(CC) $(FLAGS) -ggdb $(DEPS) -o $(OUT)
valgrind:
	$(CC) $(FLAGS)  -g -lm $(DEPS) -o $(OUT)
	valgrind --tool=memcheck --leak-check=yes ./$(OUT)
clean:
	rm -f $(OUT)

