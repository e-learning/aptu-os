CXX = g++
#CXXFLAGS = -g -ansi -Wall -pedantic
CXXFLAGS = -O2 -ansi -Wall -pedantic

EXE = shell

$(EXE):
	$(CXX) $(CXXFLAGS) shell.cpp -o $(EXE)

.PHONY: clean
clean:
	rm $(EXE)
