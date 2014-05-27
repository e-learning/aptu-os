.code16gcc

.text
.globl _start

_start:
  jmp _boot
  welcome: .asciz "Hello, World!\n\r"

  .macro mWriteString str
    leaw  \str, %si
    call .writeStringIn
  .endm

  .writeStringIn:
    lodsb
    orb  %al, %al
    jz   .writeStringOut
    movb $0x0e, %ah
    int  $0x10
    jmp  .writeStringIn
  .writeStringOut:
    ret

_boot:
  mWriteString welcome

  . = _start + 510
  .byte 0x55
  .byte 0xaa
