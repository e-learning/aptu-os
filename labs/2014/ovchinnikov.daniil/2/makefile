CC=g++
CFLAGS=-Wall
APP=scheduler
DEPS=

%.o: %.cpp $(DEPS)
	$(CC) -c $< -o $@

$(APP): main.o 
	$(CC) $^ -o $@

release: $(APP) clean_temp
	
clean_temp:
	rm -f *.o

clean: clean_temp
	rm -f $(APP)
