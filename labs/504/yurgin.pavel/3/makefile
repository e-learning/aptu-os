CXX = g++
CXXFLAGS = -std=c++11 -Wall
APPNAME = allocator
srcdir = src
builddir = ./bin

$(builddir)/allocator: $(srcdir)/main.cpp $(srcdir)/allocator.h
	$(CXX) $(CXXFLAGS) $< -o $@


clean:
	rm -rf $(builddir)/*
	

.PHONY: clean