SRC =src
CC=gcc
CFLAGS=-I$(SRC) -Wall -Werror -lm

BIN=bin

all: $(BIN) $(BIN)/sieve_unith $(BIN)/sieve_multith

$(BIN):
	mkdir $(BIN) 

$(BIN)/%.o: $(SRC)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(BIN)/sieve_unith: $(BIN)/sieve_unith.o
	$(CC) -o $@ $^ $(CFLAGS)

$(BIN)/sieve_multith: $(BIN)/sieve_multith.o
	$(CC) -o $@ $^ $(CFLAGS) -lpthread

.PHONY: clean

clean:
	rm -f $(BIN)/*