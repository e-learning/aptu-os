run: exe
        ./Allocator

exe: alloc.o
        g++ -o Allocator alloc.o
        
alloc.o:alloc.cpp
        g++ -o alloc.o -c alloc.cpp
