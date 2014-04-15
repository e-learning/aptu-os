CC = g++
CFLAGS = -Wall -ggdb -Wextra -pedantic -std=c++11

SOURCEDIR = src/utils
SOURCES = $(wildcard $(SOURCEDIR)/*.cpp)

TARGETS = init format import export ls move copy rm mkdir

all: $(TARGETS)

$(TARGETS): $(SOURCES)
	$(CC) $(CFLAGS) src/$@.cpp $(SOURCES) -o $@
	@rm -rf *.dSYM

.PHONY: clean 

clean:
	@rm -rf $(TARGETS)	
