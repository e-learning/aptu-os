NAME = loader
SRC = $(NAME).s
OBJ = $(NAME).o
BIN = $(NAME).bin

$(BIN): $(OBJ)
	ld -Ttext 0x7c00 --oformat=binary $(OBJ) -o $(BIN)

$(OBJ): $(SRC)
	as $(SRC) -o $(OBJ)

.PHONY: clean
clean:
	rm $(BIN) $(OBJ)
