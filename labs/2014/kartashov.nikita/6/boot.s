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

    movb $'!', %al
    push %ax
    movb $'d', %al
    push %ax
    movb $'l', %al
    push %ax
    movb $'r', %al
    push %ax
    movb $'o', %al
    push %ax
    movb $'W', %al
    push %ax
    movb $' ', %al
    push %ax
    movb $'o', %al
    push %ax
    movb $'l', %al
    push %ax
    movb $'l', %al
    push %ax
    movb $'e', %al
    push %ax
    movb $'H', %al
    push %ax

    movb $0x0c, %cl #loop like a B4U55

outloop:
    pop %ax
    movb $0x0e, %ah
    movb $0x00, %bh
    int  $0x10
    dec %cx
    jnz outloop
     
    movb $0x0 , %ah
    int  $0x16

    . = _start + 510
    .byte 0x55
    .byte 0xaa
