.code16
.text
    .global _start
_start:
    movw  $len,  %cx
    lea   msg,   %bx
print:
    movb  (%bx), %al
    movb  $0x0E, %ah
    int   $0x10
    inc   %bx
    dec   %cx
    jnz   print
    hlt

msg:
    .ascii "Hello, World!\r\n"
    len = . - msg

    . = _start + 510
    .byte 0x55
    .byte 0xAA
