CXX=gcc
CXXFLAGS=-Wall -pedantic -g -std=c99

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
