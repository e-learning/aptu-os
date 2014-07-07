all: bootloader.o
	ld -Ttext 0x7c00 --oformat=binary bootloader.o -o bootloader.bin
	
bootloader.o: bootloader.s
	as bootloader.s -o bootloader.o 


clean:
	rm bootloader.o
	rm bootloader.bin
	
.PHONY: clean all

