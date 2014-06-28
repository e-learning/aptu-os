bootloader: bootloader.s
	gcc bootloader.s -Xlinker -Ttext -Xlinker 0x7c00 -nostdlib -Xlinker --oformat=binary -o bootloader.bin
clean:
	rm -rf bootloader  *.bin 

