CC=g++
CFLAGS=-Wall -std=c++11
DEPS=fs.h data.h string_utils.h
APPS=init format import export ls copy move rm mkdir

%.o: %.cpp libfs.so $(DEPS) 
	$(CC) -c $< -o $@ $(CFLAGS) -lfs

all: $(APPS)
	

%: %.cpp libfs.so $(DEPS)
	$(CC) $< $(CFLAGS) -L. -lfs -o $@

libfs.so: fs.cpp $(DEPS)
	$(CC) -c $< -o fs.o $(CFLAGS) -fPIC
	$(CC) -shared -Wl,-soname,libfs.so -o libfs.so fs.o
	
clean_temp:
	rm -f *.o

clean: clean_temp
	rm -f $(APPS)
	rm -f *.so
	rm -f 1*
	rm -f 2*
	rm -f 3*
	rm -f 4*
	rm -f 5*
	rm -f 6*
	
	rm -f 8*
	rm -f 9*
	rm -f 0

