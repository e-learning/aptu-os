CXX = clang++
CXXFLAGS = -Wall -Wextra -std=c++11
APPNAME = allocator
srcdir = .
builddir = .

all: $(builddir)/main.o $(builddir)/allocator.o 
	$(CXX) $(builddir)/main.o $(builddir)/allocator.o -o $(builddir)/$(APPNAME)

$(builddir)/main.o: $(srcdir)/main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(builddir)/allocator.o: $(srcdir)/allocator.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o $(APPNAME)

run:
	$(builddir)/$(APPNAME)

.PHONY: clean