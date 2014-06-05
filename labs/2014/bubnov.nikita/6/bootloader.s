.intel_syntax
.code16gcc
.text
.globl _start
_start:

     lea %bp, begin_message

     lea %cx, end_message
     lea %ax, begin_message
     sub %cx, %ax

     mov %ax, 0
     mov %es, %ax
     
     mov %ah, 0x13
     mov %bh, 0
     mov %bl, 0x07
     mov %dh, 0
     mov %dl, 0
     int 0x10
     jmp $

begin_message:
     .asciz "hello world!"
end_message:
     . = _start + 510
     .byte 0x55
     .byte 0xaa
