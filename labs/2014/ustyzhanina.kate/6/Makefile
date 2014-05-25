default:  clean loader
loader: loader.s
	gcc loader.s -Xlinker -Ttext -Xlinker 0x7c00 -nostdlib -Xlinker --oformat=binary -o loader.bin
clean:
	rm -rf loader  *.bin 

