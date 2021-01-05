	.name tobinary
	ld hl, &prompt
	bCall(PutS)
	call &dont_check_for_decimal_points
	call &dont_check_for_negatives
	
	call &user_input
	ld hl, &FP_user_input
        bCall(Mov9ToOP1)
	
	call &convop1b
	
	push de
	push de
	
	call &toBinary

	ld hl, &binary_text
	bCall(PutS)
	bCall(NewLine)
	ld hl, &header
	bCall(PutS)
	ld hl, &binary_string
	bCall(PutS)
	ld hl, &footer
	bCall(PutS)
	
	call &toHex
	ld hl, &hex_string_0x
	bCall(PutS)
	bCall(NewLine)
	ret



	
toBinary:
	pop de
	pop hl
	push de
	;; 16 ticks
	ld (&value), hl
	ld hl, &binary_string
	
	ld a, (&value_l)

	ld b, @0x02
toBinaryLoop0:	
	push bc
;;; ======================
	ld b, @0x08
toBinaryLoop1:
	sla a
	jr nc, %no_one
	ld (hl), @0x31
no_one:
	inc hl
	djnz %toBinaryLoop1
;;; ======================
	pop bc
	ld a, (&value_h)
	djnz %toBinaryLoop0
	
	
	ret
	

value:
value_h:
.db 0x00
value_l:	
.db 0x00
binary_string:
binary_string_h:
.chars "00000000"
binary_string_l:
.chars "00000000"
.db 0x00
header:
.str "vvvv    vvvv    "
footer:
.str "    ^^^^    ^^^^"
prompt:
.str "Enter a decimal:"
binary_text:
.str "Binary:"
toHex:
	pop de
	pop hl
	push de

;;; take the 4 digits and put them into the
;;; location of the string as a byte each
	
	ld (&value), hl
	ld de, &hex_string
	ld a, h
	and @0xF0
	rrca
	rrca
	rrca
	rrca
	ld (de), a
	inc de
	ld a, h
	and @0x0F
	ld (de), a
	inc de
	ld a, l
	and @0xF0
	rrca
	rrca
	rrca
	rrca
	ld (de), a
	inc de
	ld a, l
	and @0x0F
	ld (de), a
	
;;; go through each byte and create an ASCII
;;; character out of it

	ld hl, &hex_string

	ld b, @0x04
toHexLoop:
	ld a, (hl)
		
	cp @0x0A
	jr c, %no_add
	add a, @0x07	
no_add:
	add a, @0x30
	ld (hl), a
	inc hl
	djnz %toHexLoop
	
	ret

hex_string_0x:
.chars "Hex: 0x"
hex_string:
.str "0000"
