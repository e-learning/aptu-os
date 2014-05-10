CXX = g++
CXXFLAGS = -Wall -pedantic

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

all: $(INIT) $(FORMAT) $(IMPORT) $(EXPORT) $(LS) $(COPY) $(MOVE) $(RM) $(MKDIR)

$(INIT): filesystem.o init.cpp
	$(CXX) $(CXXFLAGS) filesystem.o init.cpp -o $(INIT)

$(FORMAT): filesystem.o format.cpp 
	$(CXX) $(CXXFLAGS) filesystem.o format.cpp -o $(FORMAT)

$(IMPORT): filesystem.o import.cpp
	$(CXX) $(CXXFLAGS) filesystem.o import.cpp -o $(IMPORT)

$(EXPORT): filesystem.o export.cpp
	$(CXX) $(CXXFLAGS) filesystem.o export.cpp -o $(EXPORT)

$(LS): filesystem.o ls.cpp
	$(CXX) $(CXXFLAGS) filesystem.o ls.cpp -o $(LS)

$(COPY): filesystem.o copy.cpp
	$(CXX) $(CXXFLAGS) filesystem.o copy.cpp -o $(COPY)

$(MOVE): filesystem.o move.cpp
	$(CXX) $(CXXFLAGS) filesystem.o move.cpp -o $(MOVE)

$(RM): filesystem.o rm.cpp
	$(CXX) $(CXXFLAGS) filesystem.o rm.cpp -o $(RM)

$(MKDIR): filesystem.o mkdir.cpp
	$(CXX) $(CXXFLAGS) filesystem.o mkdir.cpp -o $(MKDIR)

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

