FLAGS=-O2 -Wall -Werror 
CC = gcc
OUT = shell
OBJDIR = obj
SRCDIR = .

all: $(OUT)
$(OBJDIR)/auxiliary.o: $(SRCDIR)/auxiliary.c $(SRCDIR)/auxiliary.h
	$(CC) $(FLAGS) -c $(SRCDIR)/auxiliary.c -o $(OBJDIR)/auxiliary.o
$(OBJDIR)/commands.o: $(SRCDIR)/commands.c $(SRCDIR)/commands.h
	$(CC) $(FLAGS) -c $(SRCDIR)/commands.c -o $(OBJDIR)/commands.o
$(OBJDIR)/shell.o: $(SRCDIR)/shell.c $(SRCDIR)/auxiliary.h $(SRCDIR)/commands.h
	$(CC) $(FLAGS) -c $(SRCDIR)/shell.c -o $(OBJDIR)/shell.o
$(OUT): $(OBJDIR)/auxiliary.o $(OBJDIR)/commands.o $(OBJDIR)/shell.o
	mkdir -p $(OBJDIR)
	$(CC) $(FLAGS) $(OBJDIR)/auxiliary.o $(OBJDIR)/commands.o $(OBJDIR)/shell.o -o $(OUT)

.PHONY: clean

clean:
	rm -f $(OBJDIR)/*.o $(OUT)
