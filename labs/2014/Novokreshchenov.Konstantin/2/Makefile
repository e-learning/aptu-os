CXX = g++
CXXFLAGS = -Wall

OUTDIR = bin

EXEC = scheduler
SOURCES = $(wildcard *.cpp)
OBJECTS := $(SOURCES:%.cpp=$(OUTDIR)/%.o)

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXEC)

$(OBJECTS): $(OUTDIR)/%.o : %.cpp
	mkdir -p $(OUTDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OUTDIR)

.PHONY: clean
