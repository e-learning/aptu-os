CXX=g++
CXXFLAGS=-Wall -Werror -pedantic -g -std=c++11

EXECUTABLE=scheduler

SOURCES=main.cpp
OBJECTS=$(patsubst %,%,$(SOURCES:.cpp=.o))

.PHONY: clean

all: $(EXECUTABLE)

clean:
	rm -rvf *.dsym $(EXECUTABLE) *.o

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(EXECUTABLE) $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -c -o $@ $<

-include *.d
