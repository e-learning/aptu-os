CXX = g++
CXXFLAGS = -std=c++11 -Wall -Werror
APPNAME = memory
srcdir = ./
builddir = ./

$(builddir)/memory: $(srcdir)/main.cpp
	$(CXX) $(CXXFLAGS) $< -o $@


clean:
	rm -rf memory
	

.PHONY: clean 
