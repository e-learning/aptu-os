CXX=g++
CXXFLAGS=-std=c++11 -Wall -pedantic -O2

CPP_FILES=$(wildcard *.cpp)
OBJ_FILES=$(CPP_FILES:.cpp=.o)

.DEFAULT_GOAL: all
	.PHONY: clean

all: $(OBJ_FILES)
	$(CXX) $(OBJ_FILES) -o allocator

%.o: %.cpp %.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm *.o ./allocator
