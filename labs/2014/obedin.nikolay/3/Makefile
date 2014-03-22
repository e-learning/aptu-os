CXX=g++
CXXFLAGS=-Wall -Werror -pedantic -g -std=c++11

EXECUTABLES=init format #import export ls copy move rm mkdir
SOURCES=filesystem.cpp file.cpp
OBJECTS=$(patsubst %,%,$(SOURCES:.cpp=.o))

.PHONY: clean codegen

all: codegen

programs: $(EXECUTABLES)

clean:
	rm -rvf *.dsym *.dSYM codegen $(EXECUTABLES) *.o

codegen:
	@$(CXX) $(CXXFLAGS) -o $@ codegen.cpp
	@./codegen init 1 path
	@./codegen format 1 path
	@make programs

$(EXECUTABLES): %: %.o $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -c -o $@ $<

-include *.d
