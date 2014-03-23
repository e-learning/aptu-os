CXX=g++-4.8
CXXFLAGS=-Wall -Werror -pedantic -g -std=c++11

EXECUTABLES=init format import ls export #copy move rm mkdir
SOURCES=filesystem.cpp file.cpp directory.cpp filebuf.cpp
OBJECTS=$(patsubst %,%,$(SOURCES:.cpp=.o))

.PHONY: clean codegen

all: codegen

programs: $(EXECUTABLES)

clean:
	rm -rvf *.dsym *.dSYM codegen $(EXECUTABLES) *.o

codegen:
	@$(CXX) $(CXXFLAGS) -o $@ codegen.cpp
	@./codegen init f 1 path
	@./codegen format f 1 path
	@./codegen import t 3 path host_file fs_file
	@./codegen export t 3 path fs_file host_file
	@make programs

$(EXECUTABLES): %: %.o $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -c -o $@ $<

-include *.d
