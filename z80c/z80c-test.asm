;; Mike Pellegrino
;; (C) 2020
;; TI8x Assembler for tias
;; Tests the Compiler's indexing instructions
.name z80ctest
	ld bc, #0x0000
00106$:
	push bc

	ld hl, &txt
	bCall(PutS)
	bCall(NewLine)
	
	pop bc
	ld a, c
	ld d, @0x06
loop_count:
	sub d
	ld a, b
	rla
	ccf
	rra
	ld d, @0x80
	sbc a, d
	jr nc, %00101$
	inc bc
	jr %00106$
00101$:
	nop
	ld hl, &txt
	ld a, @0x31
	inc hl
	inc hl
	ld (hl), a
	ld hl, &loop_count
	dec hl
	ld a, @0x05
	ld (hl), a
	ld a, @0xC9
	ld hl, &00101$
	ld (hl), a
	ld bc, #0x0000
	jp &00106$
	
	ld hl, #0x0000
	ret
txt:
.str "z80 test"
.db 0
