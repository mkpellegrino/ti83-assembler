.name hex2dec
;;; -----------------------------------------------
	call &fully_clear_screen
	
	ld hl, &text_prompt
	bCall(PutS)
	call &hex_input
	bCall(NewLine)
	ld hl, &zerox
	bCall(PutS)
	ld hl, &function_hex_input_text_bfr_start
	bCall(PutS)
	bCall(NewLine)
	
	ld hl, &function_hex_input_text_bfr_start
	ld (&ptr), hl
	call &str2hex
	ld hl, (&str2hex_tally)	
	bCall(DispHL)
	bCall(NewLine)
	
	ret
	
hex_input_acc:
.dw 0x0000
	
loop_count:
.db 0x00
exponent:
.db 0x00
text_prompt:
.str "> "
;;; -----------------------------------------------
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
	ld bc, #0x0000

;; ADDRESS OF STRING that is to be converted
	ld hl, (&ptr)
	
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
	ld hl, (&ptr)
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
	xor a
	ld (&times_sixteen_lo), a
	ld a, (&times_sixteen_value)
	ld (&times_sixteen_hi), a
	popa
	ret
	
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

ptr:
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
