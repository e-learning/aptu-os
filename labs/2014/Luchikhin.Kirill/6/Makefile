default: boot.bin

boot.bin: boot.o script.ld
	ld --oformat binary -T script.ld boot.o -o boot.bin

boot.o: boot.S
	as boot.S -o boot.o

clean:
	rm *.o boot.bin
