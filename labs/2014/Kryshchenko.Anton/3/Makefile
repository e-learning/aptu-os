CXX = g++
CXXFLAGS = -g -Wall -Wextra -std=c++11 -pedantic
APPNAME = init format import export ls copy move rm mkdir
OBJECTS = $(builddir)/filedesc.o $(builddir)/filesystem.o 
srcdir = src
builddir = obj

all: $(builddir) $(APPNAME)

$(builddir):
	mkdir -p $(builddir)

####################

$(builddir)/filedesc.o: src/filedesc.cpp src/filedesc.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(builddir)/filesystem.o: src/filesystem.cpp src/filesystem.h src/filedesc.h
	$(CXX) $(CXXFLAGS) -c $< -o $@


####################

$(builddir)/%.o: $(srcdir)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%: $(builddir)/%.o $(OBJECTS)
	$(CXX) $(CXXFLAGS) $< $(OBJECTS) -o $@

clean:
	rm -rf $(builddir)
	rm -rf $(APPNAME)
	
run:

.PHONY: clean
