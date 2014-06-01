ARCH=x86-64
NAME=boot

all: $(NAME).elf floppy.img
	objcopy -O binary $(NAME).elf $(NAME).bin
	dd if=$(NAME).bin of=floppy.img

$(NAME).elf: $(NAME).c
	gcc -Os -march=$(ARCH) -ffreestanding -Wall -Werror -g -c $(NAME).c -o $(NAME).o
	ld -static -T$(NAME).ld -nostdlib --nmagic $(NAME).o -o $(NAME).elf

floppy.img:
	dd if=/dev/zero of=floppy.img bs=512 count=2800

clean:
	rm -f *.o *.elf *.bin *.img
	
