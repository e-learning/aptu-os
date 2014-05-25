	.file	"boot.c"
#APP
	.code16

	jmpl $0x0000, $main

#NO_APP
	.section	.text.startup,"ax",@progbits
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
#APP
# 5 "boot.c" 1
	movb $'H' , %al

# 0 "" 2
# 6 "boot.c" 1
	movb $0x0e, %ah

# 0 "" 2
# 7 "boot.c" 1
	int  $0x10

# 0 "" 2
# 9 "boot.c" 1
	movb $'e' , %al

# 0 "" 2
# 10 "boot.c" 1
	movb $0x0e, %ah

# 0 "" 2
# 11 "boot.c" 1
	int  $0x10

# 0 "" 2
# 13 "boot.c" 1
	movb $'l' , %al

# 0 "" 2
# 14 "boot.c" 1
	movb $0x0e, %ah

# 0 "" 2
# 15 "boot.c" 1
	int  $0x10

# 0 "" 2
# 17 "boot.c" 1
	movb $'l' , %al

# 0 "" 2
# 18 "boot.c" 1
	movb $0x0e, %ah

# 0 "" 2
# 19 "boot.c" 1
	int  $0x10

# 0 "" 2
# 21 "boot.c" 1
	movb $'o' , %al

# 0 "" 2
# 22 "boot.c" 1
	movb $0x0e, %ah

# 0 "" 2
# 23 "boot.c" 1
	int  $0x10

# 0 "" 2
# 25 "boot.c" 1
	movb $',' , %al

# 0 "" 2
# 26 "boot.c" 1
	movb $0x0e, %ah

# 0 "" 2
# 27 "boot.c" 1
	int  $0x10

# 0 "" 2
# 29 "boot.c" 1
	movb $' ' , %al

# 0 "" 2
# 30 "boot.c" 1
	movb $0x0e, %ah

# 0 "" 2
# 31 "boot.c" 1
	int  $0x10

# 0 "" 2
# 33 "boot.c" 1
	movb $'W' , %al

# 0 "" 2
# 34 "boot.c" 1
	movb $0x0e, %ah

# 0 "" 2
# 35 "boot.c" 1
	int  $0x10

# 0 "" 2
# 37 "boot.c" 1
	movb $'o' , %al

# 0 "" 2
# 38 "boot.c" 1
	movb $0x0e, %ah

# 0 "" 2
# 39 "boot.c" 1
	int  $0x10

# 0 "" 2
# 41 "boot.c" 1
	movb $'r' , %al

# 0 "" 2
# 42 "boot.c" 1
	movb $0x0e, %ah

# 0 "" 2
# 43 "boot.c" 1
	int  $0x10

# 0 "" 2
# 45 "boot.c" 1
	movb $'l' , %al

# 0 "" 2
# 46 "boot.c" 1
	movb $0x0e, %ah

# 0 "" 2
# 47 "boot.c" 1
	int  $0x10

# 0 "" 2
# 49 "boot.c" 1
	movb $'d' , %al

# 0 "" 2
# 50 "boot.c" 1
	movb $0x0e, %ah

# 0 "" 2
# 51 "boot.c" 1
	int  $0x10

# 0 "" 2
#NO_APP
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 4.8.2-19ubuntu1) 4.8.2"
	.section	.note.GNU-stack,"",@progbits
