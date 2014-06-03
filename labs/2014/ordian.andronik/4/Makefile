CXX=g++
CXXFLAGS= -Wall -Wextra -std=c++11 -pedantic
OFLAGS=-Ofast
CPP_SRCS=main.cpp 
TARGET=allocator

all: $(TARGET)

$(TARGET): $(CPP_SRCS)
	@echo "Compiling " $@
	$(CXX) $(CXXFLAGS) $(CPP_SRCS) -o $@

clean:
	@echo "Cleaning..."
	rm -rf *~ ${TARGET}

.PHONY: clean
