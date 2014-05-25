CXX = g++
CXXFLAGS = -g -ansi -Wall -pedantic
#CXXFLAGS = -O2 -ansi -Wall -pedantic

SRC = allocator.cpp
EXE = allocator

$(EXE): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(EXE)

.PHONY: clean
clean:
	rm $(EXE)
