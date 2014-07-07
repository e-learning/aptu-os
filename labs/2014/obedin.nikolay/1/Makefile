CXX=gcc
CXXFLAGS=-Wall -Werror -pedantic -g -std=c99 -D_POSIX_SOURCE -D_POSIX_C_SOURCE=200809L

EXECUTABLE=shell

SOURCES=main.c
OBJECTS=$(patsubst %,%,$(SOURCES:.c=.o))

.PHONY: clean

all: $(EXECUTABLE)

clean:
	rm -rvf *.dsym $(EXECUTABLE) *.o

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(EXECUTABLE) $^

%.o: %.c
	$(CXX) $(CXXFLAGS) -MMD -c -o $@ $<

-include *.d
