.intel_syntax
.code16
.text                     #executable code location
.globl _start
_start:                   #code entry point

		xor %ax, %ax
		mov %es, %ax
								#очищаем экран
		mov %al, 0x02	
		mov %ah, 0x00	
		int 0x10
	 
		mov %bp,OFFSET FLAT:msg #в bp- смещение строки
		mov %ax,0x1301 #в ah - номер функции -13
		#в  al -01 - строка только с символами + смещение курсора
		mov %bx,0x0f   #bh-страница, здесь 0, bl - атрибут (синий цвет)
		mov %cx,15 #длина строки захардкожена :(
		mov %dx,0x0c20  #dh-номер строки на экране, dl - номер столбца на экране
		int 0x10
   
msg:
     .ascii "Hello, world!\n\r"

     . = _start + 510     #mov to 510th byte from 0 pos
     .byte 0x55           #append boot signature
     .byte 0xaa           #append boot signature

