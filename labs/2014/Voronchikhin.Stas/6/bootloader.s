.intel_syntax
.code16gcc
.text
.globl _start
_start:
     mov %al, 0X01
     mov %ah, 0X00 #clean
     int 0x10  #video service bios 
     mov %ax, 0
     mov %es, %ax
     mov %bp, OFFSET FLAT:message
     mov %ah, 0x13 #print string
     mov %bh, 0
     mov %bl, 150
     mov %dh, 0 
     mov %cx, 12
     mov %dl, 0
     int 0x10

message:
     .ascii "Hello World!"
     . = _start + 510
     .byte 0x55 #magic
     .byte 0xaa	
