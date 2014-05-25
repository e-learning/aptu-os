CXX = g++

CXXFLAGS = -Wall -pedantic

EXE = shell

$(EXE):
	$(CXX) $(CXXFLAGS) main.cpp Executor.cpp -o $(EXE)

.PHONY: clean
clean:
	rm $(EXE)
