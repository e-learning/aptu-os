CXX = g++
CXXFLAGS = -Wall

EXEC = shell
SOURCES = $(wildcard *.cpp)
OBJECTS  := $(SOURCES:%.cpp=%.o)

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXEC)

$(OBJECTS): %.o : %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -rf %.o


