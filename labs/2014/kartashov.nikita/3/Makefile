CC = clang++
CFLAGS = -Wall -O2 -ggdb -Wextra -pedantic -Werror -std=c++11

SOURCEDIR = src
UTILDIR = $(SOURCEDIR)/utils

UTIL_SOURCES = $(wildcard $(UTILDIR)/*.cpp)
SOURCES = $(wildcard $(SOURCEDIR)/*.cpp)

TARGETS = $(SOURCES:src/%.cpp=%)

all: $(TARGETS)

$(TARGETS): $(SOURCES) $(UTIL_SOURCES)
	@echo -n 'Linking $@ ...'
	$(CC) $(CFLAGS) $(UTIL_SOURCES) src/$@.cpp -o $@
	@echo ' OK'
	@rm -rf *.dSYM


clean:
	@rm -rf $(BINDIR) $(TARGETS)

.PHONY: clean 
