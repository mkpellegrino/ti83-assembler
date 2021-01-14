.name bytedump
	call &fully_clear_screen
	ld hl, &title
	bCall(PutS)
	call &hex_input
	pop hl
	ld (&start_addr), hl

	ld hl, &instr
	bCall(PutS)
	jr %skip
getkey:	
	bCall(GetCSC)
	cp @0x00
	jr z, %getkey
skip:
	cp @0x0F
	jr z, %end

	pusha
	ld hl, (&start_addr)
	push hl
	call &hex_to_string
	pop hl
	bCall(PutS)
	ld a, @0x20
	bCall(PutC)	
	popa

	ld hl, (&start_addr)
	ld a, (hl)

	ld b, @0x00
	ld c, a

	push bc
	call &hex_to_string
	pop hl
	inc hl
	inc hl
	bCall(PutS)
	bCall(NewLine)
	
	ld hl, (&start_addr)
	inc hl
	ld (&start_addr), hl
	
	jr %getkey
	
end:	
	ret

start_addr:
	.dw 0x0000
	
title:
;;;;; 123456789ABCDEF-123456789ABCDEF-123456789ABCDEF-123456789ABCDEF
	.str "    dump v2     by: m pellegrino----------------addr:"
instr:
	.str " :clear: to end "
	
