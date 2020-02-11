.name formulas
	;display title
	;display menu
	;get user choice
	;if invalid choice, show menu
	;if valid choice, call function
	;for the specific formula
	
	bCall(CursorOn)
	bCall(RunIndicOff)
start:
	xor a
	ld (&CurRow), a
	ld (&CurCol), a
	bCall(ClrScrnFull)
	bCall(ClrLCDFull)
	ld hl, &title_text
	bCall(PutS)
	ld hl, &menu_text
	bCall(PutS)
	ld hl, &prompt
	bCall(PutS)
	bCall(GetKey)
	bCall(NewLine)

	cp @0x8E
	jp z, &dec_to_hex
	cp @0x8F
	jp z, &float
	cp @0x90
	jp z, &getkey
	cp @0x91
	jp z, &getcsc
	cp @0x92
	jp z, &exit
	jr %start
	ret

dec_to_hex:
	ld hl, &dec_to_hex_title
	bCall(PutS)
	bCall(NewLine)

	ld hl, &prompt
	bCall(PutS)
	call &dont_check_for_negatives
	call &dont_check_for_decimal_points
	call &user_input
	call &check_for_negatives
	call &check_for_decimal_points

	bCall(ClrTxtShd)
	
	ld hl, &FP_user_input
	bCall(Mov9ToOP1)
	bCall(ConvOP1)

	; now de is the value

	ld b, @0x04
dec_to_hex_loop0:
	ld a, e
	ld (&byte_in), a
	call &convert_byte
	
	;ld a, (&char_out)
	
	; store the value in buffer + b
	ld a, b
	push bc
	push af
	ld c, a
	ld hl, &dec_to_hex_string
	inc hl
	ld b, @0x00
	add hl, bc
	ld a, (&char_out)
	ld (hl), a
	pop af
	pop bc
	
	ld hl, &hi
	ld (hl), d
	inc hl
	ld (hl), e
	call &shift_word_right
	ld e, (hl)
	dec hl
	ld d, (hl)
	
	djnz %dec_to_hex_loop0

	ld hl, &dec_to_hex_string
	bCall(PutS)
	bCall(ClrTxtShd)

	bCall(NewLine)
	bCall(GetKey)

	; re-enable decimal point if possible
	
	jp &start
	
shift_word_right:
	push af
	push bc
	push de
	push hl
	; do this 4 times
	ld b, @0x04
	ld hl, &hi
	ld d, (hl)
	inc hl
	ld e, (hl)
	
shift_word_right_loop0:
	srl e
	srl d	
	jr nc, %shift_word_right_no_carry
	set 7, e
shift_word_right_no_carry:
	djnz %shift_word_right_loop0
	ld (hl), e
	dec hl
	ld (hl), d
	
	pop hl
	pop de
	pop bc
	pop af
	ret
hi:
.db 0x00
lo:
.db 0x00
	
convert_byte:
	push hl
	push de
	push bc
	push af
	ld a, (&byte_in)
	ld b, @0x0F
	and b
	cp @0x0A
	jr nc, %convert
	jr %no_convert
convert:
	ld b, @0x07
	add a, b
no_convert:
	ld b, @0x30
	add a, b

	ld (&char_out), a
	pop af
	pop bc
	pop de
	pop hl
	ret

byte_in:
.db 0x00
	
char_out:
.db 0x00
	

.fp FP_dec
	
float:
	ld hl, &float_title
	bCall(PutS)
	ld hl, &prompt
	bCall(PutS)
	call &user_input
	ld b, @0x09
	ld de, &FP_user_input
float_loop_top:
	; Save the iteration count
	push bc
	; Put a byte into a
	ld a, (de)

	; save pointer
	push de

	
	; display that byte
	ld l, a
	ld h, @0x00
	bCall(DispHL)
	bCall(NewLine)
	; restore the pointer	
	pop de
	; move the pointer to next byte
	inc de

	; restore the iteration count
	pop bc

	;put the iteration count into a 
	ld a, b

	;compare a with 5
	cp @0x05

	;if a==5, jr to "more..." section
	jr z, %float_loop_skip

	; dec bc, cp it with zero
	djnz %float_loop_top
	jr %float_loop_done
float_loop_skip:
	push bc
	push de
	ld hl, &more_text
	bCall(PutS)
	bCall(GetKey)
	bCall(NewLine)
	pop de
	pop bc
	djnz %float_loop_top
	
float_loop_done:
	ld hl, &pak_text
	bCall(PutS)
	bCall(GetKey)
	jp &start

	
getkey:
	ld hl, &getkey_title
	bCall(PutS)
	;ld hl, &pak_text
	;bCall(PutS)
	bCall(GetKey)
	bCall(NewLine)

	ld l, a
	ld h, @0x00
	bCall(DispHL)
	bCall(NewLine)

	
	ld hl, &pak_text
	bCall(PutS)
	bCall(GetKey)


	jp &start
getcsc:
	ld hl, &getcsc_title
	bCall(PutS)

	
	ld hl, &pak_text
	bCall(PutS)	
	bCall(GetKey)
	jp &start
exit:
	bCall(ClrLCDFull)
	ret

dec_to_hex_string:
	.str "0x0000"
	
dec_to_hex_title:
;     123456789012345-123456789012345-123456789012345- 
.str "    Dec->Hex    ----------------"
float_title:
.str " Float -> Bytes ----------------"
getkey_title:
.str "     GetKey     ----------------"
getcsc_title:
.str "     GetCSC     ----------------"
pak_text:
.str "Press Any Key"
title_text:
.str "    Formulas    by mr pellegrino----------------"
;     123456789012345-123456789012345-123456789012345- 
.db 0x00
menu_text:
.str "0.Dc>Hx 1.FP>Hx 2.Key>? 3.CSC>? 4.Exit          "
more_text:
.str "more..."
prompt:
.str "> "

FP_16:
.db 0x00
.db 0x81
.db 0x16
.db 0x00
.db 0x00
.db 0x00
.db 0x00
.db 0x00
.db 0x00	
