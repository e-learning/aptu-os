CXX = g++
CXXFLAGS = -Wall -pedantic -std=c++0x

OUTDIR = bin
EXEC = memory

SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:%.cpp=$(OUTDIR)/%.o)

all: ADDDIR $(EXEC)

ADDDIR:
	mkdir -p $(OUTDIR)

$(EXEC): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(EXEC)

$(OBJECTS): $(OUTDIR)/%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	rm -rf $(EXEC)
	rm -rf $(OUTDIR)


.PHONY: clean
