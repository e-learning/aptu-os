CXX = g++
CXXFLAGS = -Wall -pedantic -std=c++0x

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
OBJECTS = $(SOURCES:%.cpp=$(OUTDIR)/%.o)

all: ADDDIR $(INIT) $(FORMAT) $(IMPORT) $(EXPORT) $(LS) $(COPY) $(MOVE) $(RM) $(MKDIR)

ADDDIR:
	mkdir -p $(OUTDIR)

$(INIT): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) ./mains/init.cpp -o $(INIT)

$(FORMAT): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) ./mains/format.cpp -o $(FORMAT)

$(IMPORT): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) ./mains/import.cpp -o $(IMPORT)

$(EXPORT): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) ./mains/export.cpp -o $(EXPORT)

$(LS): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) ./mains/ls.cpp -o $(LS)

$(COPY): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) ./mains/copy.cpp -o $(COPY)

$(MOVE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) ./mains/move.cpp -o $(MOVE)

$(RM): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) ./mains/rm.cpp -o $(RM)

$(MKDIR): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) ./mains/mkdir.cpp -o $(MKDIR)

$(OBJECTS): $(OUTDIR)/%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

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


.PHONY: clean
