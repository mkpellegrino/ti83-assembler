.name formulas
	;display title
	;display menu
	;get user choice
	;if invalid choice, show menu
	;if valid choice, call function
	;for the specific formula

	
	bCall(RunIndicOff)
start:
	call &fully_clear_screen
	
	ld hl, &title_text
	bCall(PutS)
	ld hl, &menu_text
	bCall(PutS)
	ld hl, &prompt
	bCall(PutS)
	bCall(CursorOn)
	bCall(GetKey)
	bCall(CursorOff)
	bCall(NewLine)

	cp @0x8E
	jp z, &exit
	cp @0x8F
	jp z, &dec_to_hex
	cp @0x90
	jp z, &float
	cp @0x91
	jp z, &hex2dec
	cp @0x92
	jp z, &getkey
	cp @0x93
	jp z, &getcsc
	cp @0x94
	jr z, %disassemble
	cp @0x95
	jr z, %dectochar
	jr %start
	ret

dectochar:
	pusha
	ld hl, &prompt
	bCall(PutS)
	call &user_input
	call &convop1b
	push de
	ld a, (&lb)
	bCall(PutC)
	pop hl
	ld a, l
	bCall(PutC)
	ld a, (&rb)
	bCall(PutC)
	bCall(GetKey)
	popa
	jr %start
lb:
.db 0x7B
rb:
.db 0x7D	
	
disassemble:
	call &fully_clear_screen

	ld hl, &disassemble_title
	bCall(PutS)
	bCall(NewLine)

	ld hl, &prompt
	bCall(PutS)

	call &dont_check_for_negatives
	call &dont_check_for_decimal_points
	call &user_input
	ld hl, &FP_user_input
	bCall(Mov9ToOP1)
	
	;bCall(ConvOP1)
	call &convop1b
	ld h, d
	ld l, e
	
disassemble_loop:	
	ld a, (hl)
	push hl
	ld h, @0x00
	ld l, a
	bCall(DispHL)
	bCall(NewLine)
	
	bCall(GetKey)
	pop hl
	inc hl

	cp @0x8D
	jp z, &start
	inc hl
	jr %disassemble_loop

	
dec_to_hex:
	call &fully_clear_screen

	ld hl, &dec_to_hex_title
	bCall(PutS)
	bCall(NewLine)


	ld hl, &prompt
	bCall(PutS)
	call &dont_check_for_negatives
	call &dont_check_for_decimal_points
	call &user_input
	ld hl, &FP_user_input
	bCall(Mov9ToOP1)
	;bCall(ConvOP1)
	call &convop1b
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

	call &check_for_negatives
	call &check_for_decimal_points
	
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

	; Convert a HEXADECIMAL digit to ASCII character
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
		
float:
	call &fully_clear_screen

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
	call &fully_clear_screen

	ld hl, &getkey_title
	bCall(PutS)

	ld hl, &prompt
	bCall(PutS)
	bCall(CursorOn)
	bCall(GetKey)
	bCall(CursorOff)
	bCall(NewLine)

	ld hl, &a_equals
	bCall(PutS)

	ld l, a
	ld h, @0x00
	bCall(DispHL)
	bCall(NewLine)
	
	ld hl, &pak_text
	bCall(PutS)
	bCall(GetKey)

	jp &start
getcsc:
	call &fully_clear_screen
	
	ld hl, &getcsc_title
	bCall(PutS)

	ld hl, &prompt
	bCall(PutS)

	bCall(CursorOn)

getcsc_get_byte:
	bCall(GetCSC)
	bCall(CursorOff)
	
	cp @0x00
	jr z, %getcsc_get_byte

	ld hl, &a_equals
	bCall(PutS)

	ld l, a
	ld h, @0x00
	bCall(DispHL)
	bCall(NewLine)

	ld hl, &pak_text
	bCall(PutS)	
	bCall(GetKey)
	jp &start
exit:
	call &fully_clear_screen
	
	ret

hex2dec:
	pusha
	; clear out the tally (we'll have a tally of fun)
	ld hl, #0x0000
	ld (&str2hex_tally), hl
	
	call &fully_clear_screen
	ld hl, &prompt
	bCall(PutS)
	
	call &hex_input

	bCall(NewLine)

	ld hl, &zerox
	bCall(PutS)

	ld hl, &function_hex_input_text_bfr_start
	bCall(PutS)
	bCall(NewLine)
	
	ld hl, &function_hex_input_text_bfr_start
	ld (&str2hex_ptr), hl

	call &str2hex

	ld hl, (&str2hex_tally)	

	bCall(DispHL)
	bCall(NewLine)
	bCall(GetKey)
	popa
	jp &start


hex_input:
	pusha
	bCall(CursorOn)
	ld hl, &function_hex_input_text_bfr_start
	ld (&function_hex_input_text_bfr_ptr), hl

	;; clear out the 4-byte buffer
	ld bc, #0x0400
function_hex_input_clear_bfr_top:
	ld (hl), @0x00
	inc hl
	djnz %function_hex_input_clear_bfr_top

;; zero out the number of characters typed in
	ld hl, &function_hex_input_text_bfr_size
	ld (hl), @0x00

function_hex_input_top:

	;; if 4 characters have been typed then return
	ld a, (&function_hex_input_text_bfr_size)
	cp @0x04
	jp nc, &function_hex_input_return

function_hex_input_getscan:
	bCall(GetCSC)	
	cp @0x00
	jr z, %function_hex_input_getscan

	; save the keycode immediately
	ld (&function_hex_input_key), a

	; check for valid HEXADECIMAL digits
	cp @0x09
	jp z, &function_hex_input_cr
	cp @0x38
	jp z, &function_hex_input_delete
	cp @0x02
	jp z, &function_hex_input_delete
	cp @0x21
	jp z, &function_hex_input_digit
	cp @0x22
	jp z, &function_hex_input_digit
	cp @0x1A
	jp z, &function_hex_input_digit
	cp @0x12
	jp z, &function_hex_input_digit
	cp @0x23
	jp z, &function_hex_input_digit
	cp @0x1B
	jp z, &function_hex_input_digit
	cp @0x13
	jp z, &function_hex_input_digit
	cp @0x24
	jp z, &function_hex_input_digit
	cp @0x1C
	jp z, &function_hex_input_digit
	cp @0x14
	jp z, &function_hex_input_digit
	cp @0x2F
	jp z, &function_hex_input_digit
	cp @0x27
	jp z, &function_hex_input_digit
	cp @0x1F
	jp z, &function_hex_input_digit
	cp @0x2E
	jp z, &function_hex_input_digit
	cp @0x26
	jp z, &function_hex_input_digit
	cp @0x1E
	jp z, &function_hex_input_digit
	jr %function_hex_input_getscan
;;; --------------- end of scan-loop -----------

function_hex_input_lookuptable:
.str "369-???.258?FC?0147?EB??????DA"
	
function_hex_input_return:
	ld a, (&function_hex_input_text_bfr_size)
	cp @0x00
	jr z, %function_hex_input_return_dontstore
	
function_hex_input_return_dontstore:
	bCall(CursorOff)
	popa
	ret

function_hex_input_storeanddisplay:
	push af
	push bc
	push hl

	ld a, (&function_hex_input_key)
	ld c, @0x12
	sub c
	ld c, a
	ld b, @0x00
	ld hl, &function_hex_input_lookuptable
	add hl, bc

	ld a, (hl)
	ld hl, (&function_hex_input_text_bfr_ptr)
	ld (hl), a

	; =========================================================
	bCall(PutC)
	ld hl, (&function_hex_input_text_bfr_ptr)
	inc hl
	ld (&function_hex_input_text_bfr_ptr), hl
	
	ld a, (&function_hex_input_text_bfr_size)
	inc a
	ld (&function_hex_input_text_bfr_size), a
	pop hl
	pop bc
	pop af
	ret
	
	; =====================================================================
function_hex_input_delete:
	ld a, (&function_hex_input_text_bfr_size)
	cp @0x00
	jp z, &function_hex_input_top

	ld hl, &function_hex_input_text_bfr_ptr
	dec (hl)
	ld hl, (&function_hex_input_text_bfr_ptr)
	ld (hl), @0x00
 
	ld a, (&function_hex_input_text_bfr_size)
	dec a
	ld (&function_hex_input_text_bfr_size), a

	ld hl, (&CurCol)
	dec hl
	ld (&CurCol), hl

	ld a, @0x20
	
	bCall(PutC)
	ld hl, (&CurCol)
	dec hl
	ld (&CurCol), hl
	jp &function_hex_input_top
  
	; =====================================================================
function_hex_input_digit:
	call &function_hex_input_storeanddisplay
	jp &function_hex_input_top
	;=====================================================================
function_hex_input_cr:
	jr %function_hex_input_return
	;=====================================================================

;;;; =======

str2hex:
	pusha
	ld hl, #0x0000
	ld bc, hl

;; ADDRESS OF STRING that is to be converted
	ld hl, (&str2hex_ptr)
	
str2hex_loop_top:
	ld a, (hl)	
	cp @0x00
	jr z, %str2hex_len_done
	inc b
	inc hl
	jr %str2hex_loop_top
str2hex_len_done:
	ld a, b
	dec a
	ld (&times_sixteen_exponent), a
	ld hl, (&str2hex_ptr)
str2hex_loop2:
	push bc
	ld a, (&times_sixteen_exponent)
	ld a, (hl)
	ld (&times_sixteen_char), a
	call &times_sixteen_to_the_x
	inc hl
	ld a, (&times_sixteen_exponent)
	dec a
	ld (&times_sixteen_exponent), a
	push hl
	push bc
	ld hl, (&times_sixteen_hi)
	ld bc, hl
	ld hl, (&str2hex_tally)
	add hl, bc
	ld (&str2hex_tally), hl
	pop bc
	pop hl
	pop bc
	djnz %str2hex_loop2
	popa
	ret

	
times_sixteen_to_the_x:
	pusha
	ld a, (&times_sixteen_char)
	cp @0x40
	jr c, %exp_loop_lt0x40
	sub @0x07
exp_loop_lt0x40:
	sub @0x30
	ld (&times_sixteen_value), a
	ld (&times_sixteen_lo), a
	ld a, (&times_sixteen_exponent)
	cp @0x00
	jr z, %skip_it_all
	ld b, a
	ld c, @0x00
	ld hl, (&times_sixteen_value)	
exp_loop:
	push bc
	ld b, @0x04
	ld c, @0x00
exp_loopB:
	sla l
	rl h
	djnz %exp_loopB
	pop bc
	djnz %exp_loop
	ld a, (&times_sixteen_exponent)
	ld (&times_sixteen_hi), hl
	popa
	ret
skip_it_all:
	ld a, @0x00
	;xor a
	ld (&times_sixteen_lo), a
	ld a, (&times_sixteen_value)
	ld (&times_sixteen_hi), a
	popa
	ret

	

dec_to_hex_string:
.str "0x0000"

a_equals:
.str "a="
	
dec_to_hex_title:
;     123456789012345-123456789012345-123456789012345-	
.str "    Dec->Hex    ----------------"
float_title:
.str " Float -> Bytes ----------------"
getkey_title:
.str "     GetKey     ----------------"
getcsc_title:
.str "     GetCSC     ----------------"
disassemble_title:
.str "disassemble ti8x----------------"
disassemble_prompt:
.str "start> "
pak_text:
.str "Press Any Key"
title_text:
.str "    Formulas    by mr pellegrino----------------"
;     123456789012345-123456789012345-123456789012345-123456789012345- 
.db 0x00
menu_text:
.str "0.Exit  1.Dc>Hx 2.FP>Bt 3.Hx>Dc 4.Key>? 5.CSC>? 6.Diss> 7.Dc>Ch "
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

;hex_input_acc:
;.dw 0x0000
	
;loop_count:
;.db 0x00
exponent:
.db 0x00

times_sixteen_char:
.db 0x00
times_sixteen_value:
.db 0x00
times_sixteen_hi:
.db 0x00
times_sixteen_lo:
.db 0x00
times_sixteen_exponent:
.db 0x00

str2hex_tally:
.dw 0x0000

str2hex_ptr:
.dw 0x0000
	


;;;; =======

	
function_hex_input_text_bfr_size:
.db 0x00
function_hex_input_text_bfr_ptr:
.dw 0x0000
zerox:
.str "0x"
function_hex_input_text_bfr_start:
.db 0x30
.db 0x30
.db 0x30
.db 0x30
.db 0x00
function_hex_input_key:
.db 0x00
