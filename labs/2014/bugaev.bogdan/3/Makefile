CXX = g++
CXXFLAGS = -g -ansi -Wall -pedantic
#CXXFLAGS = -O2 -ansi -Wall -pedantic

SRCDIR = src
BINDIR = bin
DDIR = dependencies

VPATH = $(SRCDIR):$(BINDIR):$(DDIR)

SOURCES = $(patsubst $(SRCDIR)/%, %, $(wildcard $(SRCDIR)/*.cpp))
OBJECTS = $(SOURCES:.cpp=.o)
DS = $(SOURCES:.cpp=.d)

INIT = init
INITOBJECTS = path.o config.o init.o
FORMAT = format
FORMATOBJECTS = path.o config.o page.o file.o format.o
MKDIR = mkdir
MKDIROBJECTS = path.o config.o page.o file.o header.o mkdir.o
LS = ls
LSOBJECTS = path.o config.o page.o file.o header.o ls.o
IMPORT = import
IMPORTOBJECTS = path.o config.o page.o file.o header.o import.o
EXPORT = export
EXPORTOBJECTS = path.o config.o page.o file.o header.o export.o
RM = rm
RMOBJECTS = path.o config.o page.o file.o header.o rm.o
COPY = copy
COPYOBJECTS = path.o config.o page.o file.o header.o copy.o
MOVE = move
MOVEOBJECTS = path.o config.o page.o file.o header.o move.o

all: $(INIT) $(FORMAT) $(MKDIR) $(LS) $(IMPORT) $(EXPORT) $(RM) $(DDIR) $(RM) $(COPY) $(MOVE) $(DS)
	@cp $(BINDIR)/$(INIT) $(INIT)
	@cp $(BINDIR)/$(FORMAT) $(FORMAT)
	@cp $(BINDIR)/$(MKDIR) $(MKDIR)
	@cp $(BINDIR)/$(LS) $(LS)
	@cp $(BINDIR)/$(IMPORT) $(IMPORT)
	@cp $(BINDIR)/$(EXPORT) $(EXPORT)
	@cp $(BINDIR)/$(RM) $(RM)
	@cp $(BINDIR)/$(COPY) $(COPY)
	@cp $(BINDIR)/$(MOVE) $(MOVE)

$(INIT): $(BINDIR) $(OBJECTS)
	$(CXX) $(addprefix $(BINDIR)/, $(INITOBJECTS)) -o $(BINDIR)/$(INIT)
$(FORMAT): $(BINDIR) $(OBJECTS)
	$(CXX) $(addprefix $(BINDIR)/, $(FORMATOBJECTS)) -o $(BINDIR)/$(FORMAT)
$(MKDIR): $(BINDIR) $(OBJECTS)
	$(CXX) $(addprefix $(BINDIR)/, $(MKDIROBJECTS)) -o $(BINDIR)/$(MKDIR)
$(LS): $(BINDIR) $(OBJECTS)
	$(CXX) $(addprefix $(BINDIR)/, $(LSOBJECTS)) -o $(BINDIR)/$(LS)
$(IMPORT): $(BINDIR) $(OBJECTS)
	$(CXX) $(addprefix $(BINDIR)/, $(IMPORTOBJECTS)) -o $(BINDIR)/$(IMPORT)
$(EXPORT): $(BINDIR) $(OBJECTS)
	$(CXX) $(addprefix $(BINDIR)/, $(EXPORTOBJECTS)) -o $(BINDIR)/$(EXPORT)
$(RM): $(BINDIR) $(OBJECTS)
	$(CXX) $(addprefix $(BINDIR)/, $(RMOBJECTS)) -o $(BINDIR)/$(RM)
$(COPY): $(BINDIR) $(OBJECTS)
	$(CXX) $(addprefix $(BINDIR)/, $(COPYOBJECTS)) -o $(BINDIR)/$(COPY)
$(MOVE): $(BINDIR) $(OBJECTS)
	$(CXX) $(addprefix $(BINDIR)/, $(MOVEOBJECTS)) -o $(BINDIR)/$(MOVE)


%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $(BINDIR)/$@

$(BINDIR):
	@mkdir -p $(BINDIR)


$(DDIR):
	@mkdir -p $(DDIR)

%.d: %.cpp
	@$(CXX) -MM $< -o $(DDIR)/$@


.PHONY: clean
clean:
	rm -rf $(INIT) $(FORMAT) $(MKDIR) $(LS) $(IMPORT) $(EXPORT) $(BINDIR) $(RM) $(COPY) $(MOVE) $(DDIR)


-include $(addprefix $(DDIR)/, $(DS))
