.code16
.text
.globl _start;

_start:
        cli
        movw %cs, %ax
        movw %ax, %ds
        movw %ax, %ss
        movw $0x7C00, %sp
        sti

helloworld_txt: .ascii "Hello, world!"
.set len, .-helloworld_txt

        lea helloworld_txt, %si
        movb $len, %ecx

Print:
        lodsb
        movb $0x0E, %ah
        int  $0x10
        loop Print       
        hlt
    

. = _start + 510
.byte 0x55
.byte 0xAA
