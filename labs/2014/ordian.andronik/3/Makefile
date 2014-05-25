CXX=g++
CXXFLAGS= -Wall -Wextra -std=c++11 -pedantic
OFLAGS=
CPP_SRCS=$(wildcard src/*.cpp) 
H_SRCS=$(wildcard include/*.h)
EXEC_FILES=$(CPP_SRCS:src/%.cpp=%)
TARGET=$(EXEC_FILES)

all: $(TARGET)

$(TARGET): $(CPP_SRCS) $(H_SRCS)
	@echo "Compiling " $@
	$(CXX) $(CXXFLAGS) src/$@.cpp -o $@

clean:
	@echo "Cleaning..."
	rm -rf src/*~ include/*~ *~ ${TARGET}

.PHONY: clean
