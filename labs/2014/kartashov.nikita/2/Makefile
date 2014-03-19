CC = g++
CFLAGS = -Wall -O2 -ggdb -std=c++11

BINDIR = bin
SOURCEDIR = src
OBJDIR = $(BINDIR)/obj

SOURCES = $(wildcard $(SOURCEDIR)/*.cpp)
OBJECTS = $(patsubst $(SOURCEDIR)/%, $(OBJDIR)/%, $(patsubst %.cpp, %.o, $(SOURCES)))

TARGET = scheduler

$(TARGET): init $(OBJECTS)
	@echo -n 'Linking $@ ...'
	$(CC) $(CFLAGS) $(OBJECTS) -o $@
	@echo ' OK'

$(OBJDIR)/%.o: $(SOURCEDIR)/%.cpp
	@echo -n 'Compiling $@ ...'
	@$(CC) -c $(CFLAGS) $< -o $@
	@echo ' OK'

test:
	@tests/run_tests.sh

.PHONY: clean init test

init:
	@mkdir -p $(OBJDIR)

clean:
	@rm -rf $(BINDIR) $(TARGET)	
