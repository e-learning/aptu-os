CXX = clang++
CXXFLAGS = -Wall -Wextra -std=c++11
APPNAME = memory
srcdir = .
builddir = .

all: $(builddir)/main.o
	$(CXX) $(builddir)/main.o -o $(builddir)/$(APPNAME)

$(builddir)/main.o: $(srcdir)/main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o $(APPNAME)

run:
	$(builddir)/$(APPNAME)

.PHONY: clean