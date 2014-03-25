srcdir=src
bindir=.
objdir=obj

execs=stat init format import export mkdir ls copy move rm

VPATH = $(srcdir) $(bindir) $(objdir) 
CXX=g++
CXXFLAGS=-O2 -std=c++11 -Wall -pedantic -Wextra -Werror

objects=myfs.o 

all: $(objdir) $(execs)

$(objdir):
	mkdir -p $(objdir)

-include Makefile.dep
Makefile.dep: $(srcdir)/*.cc
	$(CXX) $(CXXFLAGS) -MM  $(srcdir)/*.cc | sed -e 's/^\(\S.*\)/\1/' > $@

%.o: %.cc 
	$(CXX) $(CXXFLAGS) -c $< -o $(objdir)/$@	

%: %.o $(objects) main.o  
	$(CXX) $(CXXFLAGS) $(objdir)/$@.o $(addprefix $(objdir)/, $(objects)) obj/main.o -o $(bindir)/$@

.PHONY: clean

clean:
	rm -rf $(execs) 
	rm -rf obj/
	rm -rf Makefile.dep
