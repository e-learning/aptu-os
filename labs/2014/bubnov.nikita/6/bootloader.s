.intel_syntax
.code16gcc
.text
.globl _start
_start:
     mov %al, 0X01
     mov %ah, 0X00
     int 0x10
     mov %ax, 0
     mov %es, %ax
     mov %bp, OFFSET FLAT:message
     mov %ah, 0x13
     mov %bh, 0
     mov %bl, 0x07
     mov %cx, 12
     mov %dh, 0
     mov %dl, 0
     int 0x10
     jmp $

message:
     .ascii "hello world!"
     . = _start + 510
     .byte 0x55
     .byte 0xaa	
