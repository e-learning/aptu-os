CXX = g++
CXXFLAGS = -g -ansi -Wall -pedantic -std=c++11
#CXXFLAGS = -O2 -ansi -Wall -pedantic -std=c++11

EXE = memory
SRC = $(EXE).cpp

$(EXE): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(EXE)

.PHONY: clean
clean:
	rm $(EXE)
