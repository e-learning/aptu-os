CPP=g++-4.8
CPP_FLAGS=-Wall -Werror -std=c++11

BIN_DIR=bin

FS=$(BIN_DIR)/file_t.o $(BIN_DIR)/directory_t.o $(BIN_DIR)/fs_t.o $(BIN_DIR)/file_buffer.o

UTILS_SRC=utils
OBJECTS=$(BIN_DIR)/init.o $(BIN_DIR)/format.o $(BIN_DIR)/import.o $(BIN_DIR)/export.o $(BIN_DIR)/ls.o $(BIN_DIR)/copy $(BIN_DIR)/move $(BIN_DIR)/rm $(BIN_DIR)/mkdir
EXECUTABLES=init format import export ls copy move rm mkdir

all: $(EXECUTABLES)

#fs
$(BIN_DIR)/file_t.o: file_t.h file_t.cpp
	$(CPP) $(CPP_FLAGS) -MMD -c file_t.cpp -o $(BIN_DIR)/file_t.o

$(BIN_DIR)/directory_t.o: directory_t.h directory_t.cpp file_t.h
	$(CPP) $(CPP_FLAGS) -MMD -c directory_t.cpp -o $(BIN_DIR)/directory_t.o

$(BIN_DIR)/file_buffer.o: file_buffer.h file_buffer.cpp fs_t.h
	$(CPP) $(CPP_FLAGS) -MMD -c file_buffer.cpp -o $(BIN_DIR)/file_buffer.o

$(BIN_DIR)/fs_t.o: fs_t.h fs_t.cpp directory_t.h
	$(CPP) $(CPP_FLAGS) -MMD -c fs_t.cpp -o $(BIN_DIR)/fs_t.o

#objects
$(BIN_DIR)/init.o: fs_t.h $(UTILS_SRC)/init.cpp
	$(CPP) $(CPP_FLAGS) -MMD -c $(UTILS_SRC)/init.cpp -o $(BIN_DIR)/init.o

$(BIN_DIR)/format.o: fs_t.h $(UTILS_SRC)/format.cpp
	$(CPP) $(CPP_FLAGS) -MMD -c $(UTILS_SRC)/format.cpp -o $(BIN_DIR)/format.o

$(BIN_DIR)/import.o: fs_t.h $(UTILS_SRC)/import.cpp
	$(CPP) $(CPP_FLAGS) -MMD -c $(UTILS_SRC)/import.cpp -o $(BIN_DIR)/import.o

$(BIN_DIR)/export.o: fs_t.h $(UTILS_SRC)/export.cpp
	$(CPP) $(CPP_FLAGS) -MMD -c $(UTILS_SRC)/export.cpp -o $(BIN_DIR)/export.o

$(BIN_DIR)/ls.o: fs_t.h $(UTILS_SRC)/ls.cpp
	$(CPP) $(CPP_FLAGS) -MMD -c $(UTILS_SRC)/ls.cpp -o $(BIN_DIR)/ls.o

$(BIN_DIR)/copy.o: fs_t.h $(UTILS_SRC)/copy.cpp
	$(CPP) $(CPP_FLAGS) -MMD -c $(UTILS_SRC)/copy.cpp -o $(BIN_DIR)/copy.o

$(BIN_DIR)/move.o: fs_t.h $(UTILS_SRC)/move.cpp
	$(CPP) $(CPP_FLAGS) -MMD -c $(UTILS_SRC)/move.cpp -o $(BIN_DIR)/move.o

$(BIN_DIR)/rm.o: fs_t.h $(UTILS_SRC)/rm.cpp
	$(CPP) $(CPP_FLAGS) -MMD -c $(UTILS_SRC)/rm.cpp -o $(BIN_DIR)/rm.o

$(BIN_DIR)/mkdir.o: fs_t.h $(UTILS_SRC)/mkdir.cpp
	$(CPP) $(CPP_FLAGS) -MMD -c $(UTILS_SRC)/mkdir.cpp -o $(BIN_DIR)/mkdir.o

#executables
init: $(FS) $(BIN_DIR)/init.o
	$(CPP) $(FS) $(BIN_DIR)/init.o -o init

format: $(FS) $(BIN_DIR)/format.o
	$(CPP) $(FS) $(BIN_DIR)/format.o -o format

import: $(FS) $(BIN_DIR)/import.o
	$(CPP) $(FS) $(BIN_DIR)/import.o -o import

export: $(FS) $(BIN_DIR)/export.o
	$(CPP) $(FS) $(BIN_DIR)/export.o -o export

ls: $(FS) $(BIN_DIR)/ls.o
	$(CPP) $(FS) $(BIN_DIR)/ls.o -o ls

copy: $(FS) $(BIN_DIR)/copy.o
	$(CPP) $(FS) $(BIN_DIR)/copy.o -o copy

move: $(FS) $(BIN_DIR)/move.o
	$(CPP) $(FS) $(BIN_DIR)/move.o -o move

rm: $(FS) $(BIN_DIR)/rm.o
	$(CPP) $(FS) $(BIN_DIR)/rm.o -o rm

mkdir: $(FS) $(BIN_DIR)/mkdir.o
	$(CPP) $(FS) $(BIN_DIR)/mkdir.o -o mkdir

#clean
clean:
	rm -rf $(BIN_DIR)/*
	rm -f $(EXECUTABLES)

.PHONY: clean