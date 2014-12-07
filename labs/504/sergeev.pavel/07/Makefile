CC=gcc
CFLAGS=-Wall -Werror

all: traceroute

traceroute: main.c
	$(CC) $(CFLAGS) main.c -o traceroute

.PHONY: clean

clean:
	rm traceroute