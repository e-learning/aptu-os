.code16
.text
    .global _start
_start:
    movw  $len,  %cx
    lea   msg,   %si
prnt:
    lodsb
    movb  $0x0e, %ah
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
