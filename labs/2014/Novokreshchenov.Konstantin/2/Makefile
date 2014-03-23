CXX = g++
CXXFLAGS = -Wall

SRCDIR = src
OUTDIR = bin

EXEC = scheduler
SOURCES = $(wildcard src/*.cpp)
OBJECTS := $(SOURCES:$(SRCDIR)/%.cpp=$(OUTDIR)/%.o)

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXEC)

$(OBJECTS): $(OUTDIR)/%.o : $(SRCDIR)/%.cpp
	mkdir -p $(OUTDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OUTDIR)

.PHONY: clean
