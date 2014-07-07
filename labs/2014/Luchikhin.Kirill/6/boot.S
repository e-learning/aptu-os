.code16
.intel_syntax noprefix
.text
.globl _start

_start:
	cli
	mov ax, cs
	mov ds, ax
	mov ss, ax
	mov sp, 0x7C00
	sti
	
	lea si, helloworld_msg
	call PrintString
	call Reboot

.func PrintString
PrintString:
	lodsb
	or al, al
	jz PrintString_Done
	
	mov ah, 0xE
	mov bx, 9
	int 0x10
	
	jmp PrintString

PrintString_Done:
	retw
.endfunc

.func Reboot
Reboot:
	lea si, reboot_msg
	call PrintString
	mov ax, 0
	int 0x16
	
	.byte 0xEA
	.word 0x0000
	.word 0xFFFF
.endfunc

helloworld_msg: .asciz "Hello, world!\r\n"
reboot_msg: .asciz "Press any key to reboot..."

. = _start + 510
.byte 0x55
.byte 0xAA
