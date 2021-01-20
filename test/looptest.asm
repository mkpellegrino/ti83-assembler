.name looptest

;;; this would be called from the main program
;;; it stores the addres of what is to be looped
;;; in where_to
;;; vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
;;; This is how many times to repeat the loop
	ld hl, &count
	ld (hl), @0x04
	inc hl
	ld (hl), @0x00

;;; This is the address of the function to be looped
	ld hl, &commands
	ld de, &cmd_adr
	ld a, l
	ld b, h
	ld (de), a
	inc de
	ld a, b
	ld (de), a
	call &loop
	

	ld hl, &count
	ld (hl), @0x03
	inc hl
	ld (hl), @0x00

;;; This is the address of the function to be looped
	ld hl, &commands2
	ld de, &cmd_adr
	ld a, l
	ld b, h
	ld (de), a
	inc de
	ld a, b
	ld (de), a
	call &loop


	ret

;;; This is the shell loop-code
loop:
	pusha
	ld hl, &count
	ld b, (hl)
top:
	push hl
	.db 0xCD
cmd_adr:	
	.dw 0x0000
	pop hl
	djnz %top
	popa
	ret
count:
	.dw 0x0005
where_to:
	.dw 0x0000
;;; ===================
commands:
	ld hl, (&i)
	push hl
	bCall(DispHL)
	bCall(NewLine)
	pop hl
	inc hl
	ld (&i), hl
	ret
i:
.dw 0x0007
	
commands2:
	ld hl, (&j)
	push hl
	bCall(DispHL)
	bCall(NewLine)
	pop hl
	inc hl
	ld (&j), hl
	ret
j:
.dw 0x000A
	
	
