TARGET=memory
SRC=main.cpp
COMP=g++
FLAGS=-Wall -Werror

all: $(TARGET)

$(TARGET): $(SRC)
	$(COMP) $(FLAGS) -std=c++0x $(SRC) -o $(TARGET)

.PHONY: clean

clean:
	rm -f $(TARGET)
