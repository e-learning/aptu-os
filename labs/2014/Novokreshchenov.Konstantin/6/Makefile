all: bootloaderHW.o
	ld -Ttext 0x7c00 --oformat=binary bootloaderHW.o -o bootloaderHW.bin

bootloaderHW.o: bootloaderHW.S
	as bootloaderHW.S -o bootloaderHW.o

clean:
	rm bootloaderHW.o
	rm bootloaderHW.bin
