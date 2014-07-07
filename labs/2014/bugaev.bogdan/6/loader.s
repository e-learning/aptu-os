.code16
.text
    .global _start
_start:
    cli
    movw  %cs,     %ax
    movw  %ax,     %ds
    addw  $0x0220, %ax 
    movw  %ax,     %ss
    movw  $0x0100, %sp
    sti

    movw  $len,    %cx
    leaw  msg,     %si
prnt:
    lodsb
    movb  $0x0e,   %ah
    int   $0x10
    dec   %cx
    jnz   prnt
    hlt

msg:
    .ascii "Hello, World!\r\n"
    len = . - msg

    . = _start + 510
    .byte 0x55
    .byte 0xaa
