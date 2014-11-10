SRC =src
CC=gcc
CFLAGS=-I$(SRC) -Wall -Werror -lm -O2

BIN=bin

all: $(BIN)/sieve_unith $(BIN)/sieve_multith

$(BIN)/%.o: $(SRC)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(BIN)/sieve_unith: $(BIN)/sieve_unith.o
	$(CC) -o $@ $^ $(CFLAGS)

$(BIN)/sieve_multith: $(BIN)/sieve_multith.o
	$(CC) -o $@ $^ $(CFLAGS) -lpthread


.PHONY: clean

clean:
	rm -f $(BIN)/*