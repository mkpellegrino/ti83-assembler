.name ascii
	call &fully_clear_screen
	bCall(RunIndicOff)
	ld hl, &title
	bCall(PutS)

ascii_loop_top:
	ld hl, &byte
	ld a, (hl)
	bCall(PutC)

	ld hl, &byte
	ld a, (hl)
	ld l, a
	ld h, @0x00
	push hl
	bCall(DispHL)
	ld a, @0x20
	bCall(PutC)
	call &hex_to_string
	pop hl
	inc hl
	inc hl
	bCall(PutS)
	bCall(NewLine)
ascii_loop_middle:
	bCall(GetKey)

	;; decimal point
	cp @0x8D
	jr z, %ascii_done

	;; right
	cp @0x01
	jr z, %byte_up

	;; left
	cp @0x02
	jr z, %byte_down

	pusha
	ld hl, &instr
	bCall(PutS)
	popa
	jr %ascii_loop_middle


byte_up:

	ld hl, &byte
	ld a, (hl)
	inc a
	ld (hl), a
	jr %ascii_loop_top

byte_down:
	ld hl, &byte
	ld a, (hl)
	dec a
	ld (hl), a
	
	jr %ascii_loop_top

ascii_done:
	
	ret

byte:
	.db 0x00
;;;            123456789ABCDEF-123456789ABCDEF-
title:
	.chars "ASCII Browser   by m pellegrino "
instr:
;;;           123456789ABCDEF-123456789ABCDEF-
	.str "left/right      decimal to end  "
