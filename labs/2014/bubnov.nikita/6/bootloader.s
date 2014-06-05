.intel_syntax
.code16gcc
.text
.globl _start
_start:


     lea %si, message
     mov %bp, %si

     mov %cx, 0
myloop:
     inc %si
     inc %cx
     mov %ax, [%si]
     cmp %ax, 0
     jne myloop

     mov %ax, 0
     mov %es, %ax
     
     mov %ah, 0x13
     mov %bh, 0
     mov %bl, 0x07
     mov %dh, 0
     mov %dl, 0
     int 0x10
     jmp $

message:
     .asciz "hello world!"
     . = _start + 510
     .byte 0x55
     .byte 0xaa
