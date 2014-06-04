all: boot_loader.o
	ld -Ttext 0x7c00 --oformat=binary boot_loader.o -o boot_loader.bin

boot_loader.o: boot_loader.s
	as boot_loader.s -o boot_loader.o

clean:
	rm boot_loader.o
	rm boot_loader.bin

.PHONY: clean all
