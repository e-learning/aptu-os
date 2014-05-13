.code16
.text
.globl _start;

_start:
	cli
	movw %cs,     %ax
	movw %ax,     %ds
	addw $0x0220, %ax
	movw %ax,     %ss
	movw $0x0100, %sp
	sti

	leaw  message, %si
	call .print
        
	.print:
		lodsb
		orb  %al, %al
		jz   .printdone
		movb $0x0e, %ah
		int  $0x10
		jmp  .print

	.printdone:
		jmp .printdone
	
  

message:
	.ascii "Hello, World!\n\r"  

	. = _start + 510
	    .byte 0x55
	    .byte 0xaa
