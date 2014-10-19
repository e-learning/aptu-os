SRC = ./src/main.cpp
OBJ = ./main.o
HPP = ./src/common.hpp
TARGET = ./scheduler
FLAGS = -Wall -Werror -std=c++11

all: $(TARGET)

$(OBJ): $(SRC) $(HPP)
	g++ $(FLAGS) -c $(SRC) -o $(OBJ)

$(TARGET): $(OBJ) $(HPP)
	g++ $(OBJ) -o $(TARGET)

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJ)
