binboot.bin: boot.s
	@gcc boot.s -Xlinker -Ttext -Xlinker 0x7c00 -nostdlib -Xlinker --oformat=binary -o $@

.PHONY: clean

clean:
	@rm boot.bin
