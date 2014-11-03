CPP=g++
CPP_FLAGS=-Werror -Wall -std=c++0x
bin=.
src=.

all: $(bin)/format $(bin)/init $(bin)/export $(bin)/import $(bin)/ls $(bin)/mkdir $(bin)/rm $(bin)/move $(bin)/copy

$(bin)/format: $(bin)/format.o $(bin)/fs.o
	$(CPP) $(CPP_FLAGS) $(bin)/format.o $(bin)/fs.o -o $(bin)/format

$(bin)/init: $(bin)/init.o $(bin)/fs.o
	$(CPP) $(CPP_FLAGS) $(bin)/init.o $(bin)/fs.o -o $(bin)/init

$(bin)/export: $(bin)/export.o $(bin)/fs.o
	$(CPP) $(CPP_FLAGS) $(bin)/export.o $(bin)/fs.o -o $(bin)/export

$(bin)/import: $(bin)/import.o $(bin)/fs.o
	$(CPP) $(CPP_FLAGS) $(bin)/import.o $(bin)/fs.o -o $(bin)/import

$(bin)/ls: $(bin)/ls.o $(bin)/fs.o
	$(CPP) $(CPP_FLAGS) $(bin)/ls.o $(bin)/fs.o -o $(bin)/ls

$(bin)/mkdir: $(bin)/mkdir.o $(bin)/fs.o
	$(CPP) $(CPP_FLAGS) $(bin)/mkdir.o $(bin)/fs.o -o $(bin)/mkdir

$(bin)/copy: $(bin)/copy.o $(bin)/fs.o
	$(CPP) $(CPP_FLAGS) $(bin)/copy.o $(bin)/fs.o -o $(bin)/copy

$(bin)/rm: $(bin)/rm.o $(bin)/fs.o
	$(CPP) $(CPP_FLAGS) $(bin)/rm.o $(bin)/fs.o -o $(bin)/rm

$(bin)/move: $(bin)/move.o $(bin)/fs.o
	$(CPP) $(CPP_FLAGS) $(bin)/move.o $(bin)/fs.o -o $(bin)/move

$(bin)/format.o:  $(src)/format.cpp $(src)/fs.cpp
	$(CPP) $(CPP_FLAGS) -c $(src)/format.cpp -o $(bin)/format.o

$(bin)/init.o:  $(src)/init.cpp $(src)/fs.cpp
	$(CPP) $(CPP_FLAGS) -c $(src)/init.cpp -o $(bin)/init.o

$(bin)/export.o:  $(src)/export.cpp $(src)/fs.cpp
	$(CPP) $(CPP_FLAGS) -c $(src)/export.cpp -o $(bin)/export.o

$(bin)/import.o:  $(src)/import.cpp $(src)/fs.cpp
	$(CPP) $(CPP_FLAGS) -c $(src)/import.cpp -o $(bin)/import.o

$(bin)/ls.o:  $(src)/ls.cpp $(src)/fs.cpp
	$(CPP) $(CPP_FLAGS) -c $(src)/ls.cpp -o $(bin)/ls.o

$(bin)/mkdir.o:  $(src)/mkdir.cpp $(src)/fs.cpp
	$(CPP) $(CPP_FLAGS) -c $(src)/mkdir.cpp -o $(bin)/mkdir.o

$(bin)/rm.o:  $(src)/rm.cpp $(src)/fs.cpp
	$(CPP) $(CPP_FLAGS) -c $(src)/rm.cpp -o $(bin)/rm.o

$(bin)/move.o:  $(src)/move.cpp $(src)/fs.cpp
	$(CPP) $(CPP_FLAGS) -c $(src)/move.cpp -o $(bin)/move.o

$(bin)/copy.o:  $(src)/copy.cpp $(src)/fs.cpp
	$(CPP) $(CPP_FLAGS) -c $(src)/copy.cpp -o $(bin)/copy.o

$(bin)/fs.o: $(src)/fs.cpp $(src)/fd.cpp $(src)/bd.cpp
	$(CPP) $(CPP_FLAGS) -c $(src)/fs.cpp -o $(bin)/fs.o

.PHONY: clean

clean:
	rm $(bin)/format $(bin)/init $(bin)/export $(bin)/import $(bin)/ls $(bin)/mkdir $(bin)/rm $(bin)/move $(bin)/copy $(bin)/format.o $(bin)/init.o $(bin)/export.o $(bin)/import.o $(bin)/ls.o $(bin)/mkdir.o $(bin)/rm.o $(bin)/move.o $(bin)/copy.o $(bin)/fs.o