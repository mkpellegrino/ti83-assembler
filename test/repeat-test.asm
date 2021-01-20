;; Mike Pellegrino
;; (C) 2020
;; TI8x Assembler for tias
;; Tests REPEAT
.name repeat
	ld a, @0x03
	; While (i<=a)
	ld hl, &loop_limit
	ld (hl), a
	; { Do ... }
	ld hl, &function_to_repeat 
	ld (&function_address), hl
	call &loop
	
	ret

loop:
	ld bc, #0x0000
00103$:
	push af
	push bc
	push de
	push hl

.db 0xCD
function_address:
	nop
	nop
	
	pop hl
	pop de
	pop bc
	pop af
	ld a, c
.db 0xD6
loop_limit:
	nop
	ld a, b
	rla
	ccf
	rra
	sbc a, @0x80
	ret nc
	inc bc
	jr %00103$

function_to_repeat:
	ld hl, &txt
	bCall(PutS)
	ld a, (&value)
	bCall(PutC)
	ld a, (&value)
	inc a
	ld (&value), a
	bCall(NewLine)
	ret

txt:
.str "repeat: "
.db 0x00
value:
.db 0x30
