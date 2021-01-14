.name hextest
	call &hex_input
	ld hl, &function_hex_input_hex_string
	bCall(PutS)
	;; equals sign
	ld a, @0x3D
	bCall(PutC)
	pop hl
	bCall(DispHL)
	bCall(NewLine)
	ld hl, &function_hex_input_hex_string
	inc hl
	inc hl
	bCall(PutS)
	
	ret

