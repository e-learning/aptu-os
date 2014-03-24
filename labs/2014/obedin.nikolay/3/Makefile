CXX=g++-4.8
CXXFLAGS=-Wall -Werror -pedantic -g -std=c++11

EXECUTABLES=init format import ls export mkdir move copy #rm
SOURCES=filesystem.cpp file.cpp directory.cpp filebuf.cpp
OBJECTS=$(patsubst %,%,$(SOURCES:.cpp=.o))

.PHONY: clean codegen

all: codegen

programs: $(EXECUTABLES)

clean:
	rm -rvf *.dsym *.dSYM codegen $(EXECUTABLES) *.o *.d

codegen:
	@$(CXX) $(CXXFLAGS) -o $@ codegen.cpp
	@./codegen init   f 1 root
	@./codegen format f 1 root
	@./codegen import t 3 root host_file fs_file
	@./codegen export t 3 root fs_file host_file
	@./codegen mkdir  t 2 root path
	@./codegen move   t 3 root from_path to_path
	@./codegen copy   t 3 root from_path to_path
	@make programs

$(EXECUTABLES): %: %.o $(OBJECTS)
	@echo "CXX $@"
	@$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	@echo "CXX $@"
	@$(CXX) $(CXXFLAGS) -MMD -c -o $@ $<

-include *.d
