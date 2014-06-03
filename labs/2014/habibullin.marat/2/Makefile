CXX = g++
CXXFLAGS = -Wall -pedantic -std=c++11

SRCDIR = .
OUTDIR = bin

EXEC = scheduler

SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OUTDIR)/%.o)

all: makedir $(EXEC)

makedir:
	mkdir $(OUTDIR)

$(EXEC): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@

$(OBJECTS): $(OUTDIR)/%.o : $(SRCDIR)/%.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	rm -rf $(OUTDIR)
	rm -rf $(EXEC)

.PHONY: clean