.code16gcc

.text
.globl _start

_start:
  cli
  movw  %cs,     %ax
  movw  %ax,     %ds
  addw  $0x0220, %ax 
  movw  %ax,     %ss
  movw  $0x0100, %sp
  sti
	
  .macro mWriteString str
    leaw  \str, %si
    call .writeStringIn
  .endm

  mWriteString welcome

  .writeStringIn:
    lodsb
    orb  %al, %al
    jz   .writeStringOut
    movb $0x0e, %ah
    int  $0x10
    jmp  .writeStringIn
  .writeStringOut:
    movb $0x0 , %ah
    int $0x16
    hlt

welcome: .asciz "Hello, World!\n\r"  	

  . = _start + 510
  .byte 0x55
  .byte 0xaa
