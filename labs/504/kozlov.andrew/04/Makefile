CPP=g++-4.8
CPP_FLAGS=-Wall -Werror -std=c++11

SOURCES=main.cpp
EXECUTABLE=memory

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	$(CPP) $(CPP_FLAGS) $(SOURCES) -o $(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE)

.PHONY: clean
