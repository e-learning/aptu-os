FLAGS=--oformat binary -T script.ld

bootloader: boot.s
	as boot.s -o boot.o
	ld $(FLAGS) boot.o -o boot.bin 

clean: 
	rm boot.o boot.bin
