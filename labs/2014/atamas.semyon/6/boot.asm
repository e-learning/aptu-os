[BITS 16]
[ORG 0x7C00]

MOV SI, HelloString
CALL NextCharacter
JMP $

PrintCharacter:
MOV AH, 0x0E
MOV BH, 0x00
MOV BL, 0x07

INT 0x10
RET

NextCharacter:
MOV AL, [SI]
INC SI
OR AL, AL
JZ ExitFunction
CALL PrintCharacter
JMP NextCharacter

ExitFunction:
RET

HelloString db 'Hello World', 0

TIMES 510 - ($ - $$) db 0
DW 0xAA55
