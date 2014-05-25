CXX = g++
CXXFLAGS = -g -ansi -Wall -pedantic
#CXXFLAGS = -O2 -ansi -Wall -pedantic

EXE = scheduler

$(EXE): scheduler.cpp
	$(CXX) $(CXXFLAGS) scheduler.cpp -o $(EXE)

.PHONY: clean
clean:
	rm $(EXE)
