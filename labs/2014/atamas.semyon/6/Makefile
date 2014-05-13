all: boot.o	
	ld -static -Tboot.ld -nostdlib --nmagic -o boot.elf boot.o
	objcopy -O binary boot.elf boot.bin
	
main: boot.s
	as -c boot.s -o boot.o
