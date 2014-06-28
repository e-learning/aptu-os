.code16
.text
.org 0x0
		.globl _start;

_start:
		movw %cs, %ax
		movw %ax, %ds
		movw %ax, %es
		movw %ax, %ss
		movw $0x7c00, %sp

		movb $'H',  %al
		movb $0x0e, %ah
		movb $0x00, %bh
		int  $0x10

		movb $'e',  %al
		movb $0x0e, %ah
		movb $0x00, %bh
		int  $0x10

		movb $'l',  %al
		movb $0x0e, %ah
		movb $0x00, %bh
		int  $0x10

		movb $'l',  %al
		movb $0x0e, %ah
		movb $0x00, %bh
		int  $0x10

		movb $'o',  %al
		movb $0x0e, %ah
		movb $0x00, %bh
		int  $0x10

		movb $',',  %al
		movb $0x0e, %ah
		movb $0x00, %bh
		int  $0x10

		movb $' ',  %al
		movb $0x0e, %ah
		movb $0x00, %bh
		int  $0x10

		movb $'W',  %al
		movb $0x0e, %ah
		movb $0x00, %bh
		int  $0x10

		movb $'o',  %al
		movb $0x0e, %ah
		movb $0x00, %bh
		int  $0x10

		movb $'r',  %al
		movb $0x0e, %ah
		movb $0x00, %bh
		int  $0x10

		movb $'l',  %al
		movb $0x0e, %ah
		movb $0x00, %bh
		int  $0x10

		movb $'d',  %al
		movb $0x0e, %ah
		movb $0x00, %bh
		int  $0x10

		movb $0x0 , %ah
		int  $0x16

		. = _start + 510

		.byte 0x55 #magic bytes
		.byte 0xaa
