CXX = g++
CXXFLAGS = -Wall -pedantic

OUTDIR = bin

INIT = init
FORMAT = format
IMPORT = import
EXPORT = export
LS = ls
COPY = copy
MOVE = move
RM = rm
MKDIR = mkdir

SOURCES = $(wildcard *.cpp)

all: ADDDIR $(INIT) $(FORMAT) $(IMPORT) $(EXPORT) $(LS) $(COPY) $(MOVE) $(RM) $(MKDIR)

ADDDIR:
	mkdir -p $(OUTDIR)

$(INIT): filesystem.o init.cpp
	$(CXX) $(CXXFLAGS) filesystem.o init.cpp -o $(OUTDIR)/$(INIT)

$(FORMAT): filesystem.o format.cpp 
	$(CXX) $(CXXFLAGS) filesystem.o format.cpp -o $(OUTDIR)/$(FORMAT)

$(IMPORT): filesystem.o import.cpp
	$(CXX) $(CXXFLAGS) filesystem.o import.cpp -o $(OUTDIR)/$(IMPORT)

$(EXPORT): filesystem.o export.cpp
	$(CXX) $(CXXFLAGS) filesystem.o export.cpp -o $(OUTDIR)/$(EXPORT)

$(LS): filesystem.o ls.cpp
	$(CXX) $(CXXFLAGS) filesystem.o ls.cpp -o $(OUTDIR)/$(LS)

$(COPY): filesystem.o copy.cpp
	$(CXX) $(CXXFLAGS) filesystem.o copy.cpp -o $(OUTDIR)/$(COPY)

$(MOVE): filesystem.o move.cpp
	$(CXX) $(CXXFLAGS) filesystem.o move.cpp -o $(OUTDIR)/$(MOVE)

$(RM): filesystem.o rm.cpp
	$(CXX) $(CXXFLAGS) filesystem.o rm.cpp -o $(OUTDIR)/$(RM)

$(MKDIR): filesystem.o mkdir.cpp
	$(CXX) $(CXXFLAGS) filesystem.o mkdir.cpp -o $(OUTDIR)/$(MKDIR)

filesystem.o: filesystem.cpp
	$(CXX) $(CXXFLAGS) -c filesystem.cpp

clean:
	rm -rf $(OUTDIR)
	rm -rf $(INIT)
	rm -rf $(FORMAT)
	rm -rf $(IMPORT)
	rm -rf $(EXPORT)
	rm -rf $(LS)
	rm -rf $(COPY)
	rm -rf $(MOVE)
	rm -rf $(RM)
	rm -rf $(MKDIR)
	rm -rf filesystem.o

.PHONY: clean

