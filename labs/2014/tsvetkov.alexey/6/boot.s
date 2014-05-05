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

msg: .ascii "hello, world"
.set len, .-msg

  lea msg, %si     
  movb $len, %ecx 
print: 
  lodsb               # load byte from si to al  
  movb $0x0e, %ah     # print byte code
  int $0x10
  loop print 

  hlt 

  . = _start + 510    # writing boot signature
  .byte 0x55
  .byte 0xaa
