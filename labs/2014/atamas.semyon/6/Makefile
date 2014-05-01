all: boot.o	
	ld -static -melf_i386 -Tboot.ld -nostdlib --nmagic -o boot.elf boot.o
	objcopy -O binary boot.elf boot.bin
	
main: boot.c
	gcc -c -g -Os -m32 -march=native -ffreestanding -Wall -Werror -o boot.o boot.c
