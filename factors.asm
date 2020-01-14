;; find the factors of a number
;; Using Brute Force
;; (C) 2017 - Michael K. Pellegrino
#include    "ti83plus.inc"
#define     progStart   $9D95
.org        progStart-2
.db         $BB,$6D

begin:
	bCall( _ZeroOP1 )	; Zero out the Last Integer Factor
	ld hl, OP1
	ld de, FP_last_integer_factor
	bCall( _Mov9B )
	
	;;; Get a Value from the User (no negatives or decimals!)
	call getuserinput
	ld hl, FP_bfr		; load source address
	ld de, FP_x		; load dest address
	bCall( _Mov9B )
	bCall( _NewLine )	; crlf

	ld hl, FP_x
	bCall( _Mov9toOP1 )

	bCall( _TimesPt5 )
	bCall( _Plus1)		; FP_x/2 + 1 -> OP1
	bCall( _Int )
	ld hl, OP1
	ld de, FP_end_at
	bCall( _Mov9B )

	bCall( _OP2Set2 )
	ld hl, OP2
	ld de, FP_d
	bCall( _Mov9B )

topofloop:

	ld hl, FP_d
	bCall( _Mov9toOP2 )

	ld hl, FP_last_integer_factor
	bCall( _Mov9toOP1 )
	
	bCall( _CpOP1OP2 )
	jr z, end

	ld hl, FP_d
	bCall( _Mov9toOP2 )	
	ld hl, FP_x
	bCall( _Mov9toOP1 )

	bCall( _FPDiv )

	ld hl, OP1 		; Save the Quotient in Memory
	ld de, FP_q
	bCall( _Mov9B )

	bCall( _Int )

	ld hl, OP1		; The Integer part of Quotient
	ld de, FP_integer
	bCall( _Mov9B )

	ld hl, FP_q
	bCall( _Mov9toOP2 )

	bCall( _FPSub )		; Now - if OP1==0, then the quotient is an integer

	bCall( _ZeroOP2 )

	bCall( _CpOP1OP2 )
	call z, disp_divisor

;; compare divisor and quotient
;; if they're the same, then end
	ld hl, FP_q
	bCall( _Mov9toOP1 )
	ld hl, FP_d
	bCall( _Mov9toOP2 )
	bCall( _CpOP1OP2 )
	jr z, end
	
;; increase divisor
	ld hl, FP_d
	bCall( _Mov9toOP1 )
	bCall( _Plus1 )
	ld hl, OP1
	ld de, FP_d
	bCall( _Mov9B )

;; Compare the Divisor with (User Input/2)+1
;; if they're = then end
	ld hl, FP_end_at
	bCall( _Mov9toOP2 )
;; OP1 already contains FP_d
	;ld hl, FP_d
	;bCall( _Mov9toOP2 )
	bCall( _CpOP1OP2 )


	jr z, end
	jr topofloop
end:	
	ret
	

readkeyA:
	push af
	push hl
readkeyA0:
	bCall(_GetCSC)		; read the keyboard
	or a			; cp a, $00
	jp z, readkeyA0
	cp sk0
	jp z, readkeyA_zero
	cp sk9
	jp z, readkeyA_nine
	cp sk8
	jp z, readkeyA_eight
	cp sk7
	jp z, readkeyA_seven
	cp sk6
	jp z, readkeyA_six
	cp sk5
	jp z, readkeyA_five
	cp sk4
	jp z, readkeyA_four
	cp sk3
	jp z, readkeyA_three
	cp sk2
	jp z, readkeyA_two
	cp sk1
	jp z, readkeyA_one
	cp skEnter
	jp z, readkeyA_cr
	cp skDecPnt
	jp z, readkeyA_backspace
	cp skDel
	jp z, readkeyA_backspace
	jp readkeyA0

readkeyA1:
	bCall( _PutC )
readkeyA2:
	ld (readkeyA_byte), a
	ld hl, (text_buffer_ptr)
	ld (hl), a
	inc hl
	ld (text_buffer_ptr), hl

	ld a, (text_buffer_length)
	inc a
	ld (text_buffer_length), a
	
	pop hl
	pop af
	ret
readkeyA_zero:
	ld a, $30
	jp readkeyA1
readkeyA_nine:
	ld a, $39
	jp readkeyA1
readkeyA_eight:
	ld a, $38
	jp readkeyA1
readkeyA_seven:
	ld a, $37
	jp readkeyA1
readkeyA_six:
	ld a, $36
	jp readkeyA1
readkeyA_five:
	ld a, $35
	jp readkeyA1
readkeyA_four:
	ld a, $34
	jp readkeyA1
readkeyA_three:
	ld a, $33
	jp readkeyA1
readkeyA_two:
	ld a, $32
	jp readkeyA1
readkeyA_one:
	ld a, $31
	jp readkeyA1
readkeyA_backspace:
	ld a, (text_buffer_length) ; if text length = 0 just return
	or a
	jp z, readkeyA0
;; ;; ; otherwise
	dec a
	ld (text_buffer_length), a

	push af
	ld a, (CurCol)
	dec a
	ld (CurCol),a
	ld a, ' '
	bCall(_PutC)
	ld a, (CurCol)
	dec a
	ld (CurCol),a
	pop af

	push hl
	ld hl, (text_buffer_ptr)
	dec hl
	ld (text_buffer_ptr), hl
	pop hl
	jp readkeyA0

readkeyA_cr:
;; check to see if this is the first byte-- if it is
;; then do nothing
	ld a, (text_buffer_length) ; if text length = 0 just return
	or a
	jp z, readkeyA0
	
	ld a, $00
	jp readkeyA2
	
;; ;; ; otherwise
	dec a
	ld (text_buffer_length), a

	push af
	ld a, (CurCol)
	dec a
	ld (CurCol),a
	ld a, ' '
	bCall(_PutC)
	ld a, (CurCol)
	dec a
	ld (CurCol),a
	pop af

	push hl
	ld hl, (text_buffer_ptr)
	dec hl
	ld (text_buffer_ptr), hl
	pop hl
	jp readkeyA0
	
create_equation:
	ld hl, equationName	; look for the variable in the vat
	bCall( _Mov9toOP1 )	; using its name (FP_VA)
	bCall( _FindSym )	; 
	jr c, storeEqu		; if it isn't found then create it	
	bCall( _DelVar )	; and delete it from the VAT
storeEqu:
	ld a, (text_buffer_length)
	ld h, $00
	ld l, a			; Bytes of Memory needed to store it
	bCall( _CreateEqu ) 	; de is returned
	inc de
	inc de
	ld hl, text_buffer	; Point hl to start of Text Buffer
	ld a, (text_buffer_length)
	ld b, $00
	ld c, a			; Bytes of Memory needed to store it
	ldir			; copy it to the VAT
	ret


store9_hl:
.dw $0000
store9_de:
.dw $0000


;; function: getuserinput
getuserinput:
	push hl
	push af
	push bc
	push de

	xor a
	ld (text_buffer_length), a
	
	ld hl, text_buffer
	ld (text_buffer_ptr), hl
	
	ld hl, prompt_text
	bCall( _PutS )
readmore:
	call readkeyA
	ld a, (text_buffer_length)
	cp $18
	jp z, buffer_filled
	
	ld a, (readkeyA_byte)
	or a
	jp z, buffer_filled
	jp readmore 

buffer_filled:
	ld a, (text_buffer_length) ; take the last byte off of the equation
	dec a
	ld (text_buffer_length), a
	call create_equation

	ld hl, equationName
	ld de, OP1
	ld bc, $04
	ldir
	bCall( _ParseInp )
	ld hl, OP1
	ld de, FP_bfr
	ld bc, $09
	ldir

	; Delete the equation from memory
	ld hl, equationName	; look for the variable in the vat
	bCall( _Mov9toOP1 )	; using its name (FP_VA)
	bCall( _FindSym )	; 
	bCall( _DelVar )	; and delete it from the VAT

	pop de
	pop bc
	pop af
	pop hl
	ret

;; display contents of OP1
disp_divisor:			
	push hl
	push af
	push bc
	push de
	ld hl, FP_d
	bCall( _Mov9toOP1 )
	ld a, $06
	bCall( _FormReal )	
	ld hl, OP3		
	bCall( _PutS )		
	ld hl, text_times
	bCall( _PutS )
	ld hl, FP_q
	bCall( _Mov9toOP1 )
	ld a, $06
	bCall( _FormReal )	
	ld hl, OP3		
	bCall( _PutS )		
	bCall( _NewLine )

	ld a, (BY_line_count)
	cp $06
	call z, pressanykey
	ld a, (BY_line_count)
	inc a
	ld (BY_line_count), a
;; save this as the last integer factor
	ld hl, FP_q
	ld de, FP_last_integer_factor
	bCall( _Mov9B )
	pop de
	pop bc
	pop af
	pop hl
	ret

;; function - pressanykey
pressanykey:
	push hl
	push af
	push de
	push bc
	ld hl, text_pressanykey
	bCall( _PutS )
	bCall( _GetKey )
	bCall( _NewLine )
	ld a, $FF
	ld (BY_line_count), a	; reset the line count
	pop bc
	pop de
	pop af
	pop hl
	ret


;; DATA SECTION
text_pressanykey:
.db "(press any key)",0

readkeyA_byte:
.db $00
equationName:
.db EquObj, tVarEqu, tY3, $00
	
text_buffer_length:
.db $00				; out of $18
	
text_buffer_ptr:
.dw $0000
	
text_buffer:			; $18 byte buffer
.db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00

FP_bfr:
.db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00

prompt_text:
.db "#> ",0

text_times:
.db " x ", 0
	
;; The user input
FP_x:
.db $00, $00, $00, $00, $00, $00, $00, $00, $00

;; the Quotient 
FP_q:
.db $00, $00, $00, $00, $00, $00, $00, $00, $00

;; the Divisor (Starts as 2)
FP_d:
.db $00, $80, $20, $00, $00, $00, $00, $00, $00

;; the int(FP_q)
FP_integer:
.db $00, $80, $20, $00, $00, $00, $00, $00, $00

FP_end_at:
.db $00, $80, $20, $00, $00, $00, $00, $00, $00

FP_last_integer_factor:
.db $00, $80, $20, $00, $00, $00, $00, $00, $00

BY_line_count:
.db $00
