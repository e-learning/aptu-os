CXX = g++
CXXFLAGS = -std=c++11 -Wall
APPNAME = allocator
srcdir = ./
builddir = ./

$(builddir)/allocator: $(srcdir)/main.cpp $(srcdir)/allocator.h
	$(CXX) $(CXXFLAGS) $< -o $@


clean:
	rm -rf allocator
	

.PHONY: clean