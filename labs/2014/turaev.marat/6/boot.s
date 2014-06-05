.code16

.text
  .globl _start

_start:
  cli
  movw %cs,     %ax
  movw %ax,     %ds
  addw $0x0220, %ax 
  movw %ax,     %ss
  movw $0x0100, %sp
  sti

.set l, .-msg
  lea msg, %si     
  movb $l, %ecx 
  
print: 
  lodsb
  movb $0x0e, %ah
  int $0x10
  loop print 
  hlt 

msg: .ascii "hello, world"

  . = _start + 510
  .byte 0x55
  .byte 0xaa
