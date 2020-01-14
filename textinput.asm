;; Numerical Input
;; For a TI8X Calculator
;; Z80 Assembler
;; (C) 2017 - Mike Pellegrino
;;
#include    "ti83plus.inc"
#define     progStart   $9D95
.org        progStart-2
.db         $BB,$6D

	ld hl, text_buffer
	ld (text_buffer_ptr), hl
	
	ld hl, prompt_text
	bCall( _PutS )
readmore:
	call readkey
	ld a, (text_buffer_length)
	cp $18
	jp z, buffer_filled
	
	ld a, (readkey_byte)
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

	bCall( _NewLine )
	ld hl, FP_bfr
	bCall( _Mov9toOP1 )
	call dispOP1
	ret

readkey:
	push af
	push hl
readkey0:
	bCall(_GetCSC)		; read the keyboard
	or a			; cp a, $00
	jp z, readkey0
	cp sk0
	jp z, readkey_zero
	cp sk9
	jp z, readkey_nine
	cp sk8
	jp z, readkey_eight
	cp sk7
	jp z, readkey_seven
	cp sk6
	jp z, readkey_six
	cp sk5
	jp z, readkey_five
	cp sk4
	jp z, readkey_four
	cp sk3
	jp z, readkey_three
	cp sk2
	jp z, readkey_two
	cp sk1
	jp z, readkey_one
	cp skEnter
	jp z, readkey_cr
	cp skDecPnt
	jp z, readkey_decpt
	cp skSub
	jp z, readkey_negative
	cp skChs
	jp z, readkey_negative
	cp skLeft
	jp z, readkey_backspace
	cp skDel
	jp z, readkey_backspace
	jp readkey0
	
readkey1:
	ld (readkey_byte), a
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
readkey_zero:
	ld a, $30
	bCall( _PutC )	
	jp readkey1
readkey_nine:
	ld a, $39
	bCall( _PutC )	
	jp readkey1
readkey_eight:
	ld a, $38
	bCall( _PutC )
	jp readkey1
readkey_seven:
	ld a, $37
	bCall( _PutC )
	jp readkey1
readkey_six:
	ld a, $36
	bCall( _PutC )
	jp readkey1
readkey_five:
	ld a, $35
	bCall( _PutC )
	jp readkey1
readkey_four:
	ld a, $34
	bCall( _PutC )
	jp readkey1
readkey_three:
	ld a, $33
	bCall( _PutC )
	jp readkey1
readkey_two:
	ld a, $32
	bCall( _PutC )
	jp readkey1
readkey_one:
	ld a, $31
	bCall( _PutC )
	jp readkey1
readkey_cr:
	ld a, $00
	jp readkey1
readkey_decpt:
	ld a, '.'
	bCall( _PutC )
	ld a, tDecPt
	jp readkey1
readkey_negative:
	ld a, '-'
	bCall( _PutC )
	ld a, tChs
	jp readkey1
readkey_backspace:
	ld a, (text_buffer_length) ; if text length = 0 just return
	or a
	jp z, readkey0
	
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
	jp readkey0
	
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
	
dispOP1:			; DEBUG
	ld a, $09		; ------ 
	bCall( _FormReal )	; DEBUG
	ld hl, OP3		; DEBUG
	bCall( _PutS )		; ------
	bCall( _NewLine )	; DEBUG
	bCall( _GetKey )	; DEBUG
	ret
	
equationName:
.db EquObj, tVarEqu, tY3, $00
	
text_buffer_length:
.db $00				; out of $18
	
text_buffer_ptr:
.dw $0000

FP_bfr:
.db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
	
text_buffer:			; $18 byte buffer
.db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00

prompt_text:
.db "> ",0
	
readkey_byte:
.db $00
