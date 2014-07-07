CXX = g++
FLAGS = -Wall -pedantic -std=c++11

SRCDIR = .
OUTDIR = bin

EXEC = allocator
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OUTDIR)/%.o)

all: init $(EXEC)

init:
	mkdir $(OUTDIR)

$(EXEC): $(OBJECTS) 
	$(CXX) $(OBJECTS) -o $@

$(OBJECTS): $(OUTDIR)/%.o : $(SRCDIR)/%.cpp
	$(CXX) -c $(FLAGS) $< -o $@

clean:
	rm -rf $(OUTDIR)
	rm -rf $(EXEC)

.PHONY: clean
