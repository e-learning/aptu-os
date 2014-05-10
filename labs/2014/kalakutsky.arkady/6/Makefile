loader: loader.s
	gcc loader.s -Xlinker -Ttext -Xlinker 0x7c00 -nostdlib -Xlinker --oformat=binary -o loader.bin
# one liner ))
#	as loader.s -o loader.o
#	ld -Ttext 0x7c00 --oformat=binary loader.o -o loader.bin

