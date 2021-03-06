#include    "ti83plus.inc"
#define     progStart   $9D95
.org        progStart-2
.db         $BB,$6D

begin:
	bCall( _RunIndicOff )
	;;; Get the Coordinates from the User
	ld hl, prompt_text_sum
	bCall( _PutS )
	call getuserinput
	ld hl, FP_bfr		; load source address
	ld de, FP_sum		; load dest address
		; copy 9 bytes src->dest
	bCall( _Mov9B )
	bCall( _NewLine )	; crlf

	ld hl, prompt_text_prod
	bCall( _PutS )
	call getuserinput
	ld hl, FP_bfr		; load source address
	ld de, FP_prod		; load dest address
		; copy 9 bytes src->dest
	bCall( _Mov9B )

	bCall( _NewLine )	; crlf

; Check to see if there are any valid solutions
	ld hl, FP_sum
	bCall( _Mov9toOP1 )	; A
	bCall( _FPSquare )	; A^2
	bCall( _TimesPt5 )	; /2
	bCall( _TimesPt5 )	; /2
	ld hl, FP_prod
	bCall( _Mov9toOP2 )	; B
	bCall( _CpOP1OP2 )	; cmp A^2/4 with B
	jr z,skip01
	jr nc,skip01

	ld hl, output_none	; "No Real Numbers"
	bCall( _PutS )
	bCall( _NewLine )	; crlf
	bCall( _RunIndicOn )

	ret

skip01:
	ld hl, FP_sum
	bCall( _Mov9toOP1 )	; A
	bCall( _FPSquare )	; A^2
	bCall( _PushOP1 )

	ld hl, FP_prod
	bCall( _Mov9toOP1 )
	bCall( _Times2 )
	bCall( _Times2 )
	bCall( _OP1ToOP2 )
	bCall( _PopOP1 )

	bCall( _FPSub )
	bCall( _SqRoot )
	
	bCall( _OP1ToOP2 )

	ld hl, FP_sum
	bCall( _Mov9toOP1 )
	bCall( _FPAdd )
	bCall( _TimesPt5 )
	call dispOP1		; Display x1
	ld hl, OP1
	ld de, FP_x1
	bCall( _Mov9B )

	ld a, tX		; Save it to User Variable X
	ld (variabletoken), a
	ld hl, FP_x1
	call storeVariable

	ld hl, FP_x1
	bCall( _Mov9toOP2 )
	ld hl, FP_prod
	bCall( _Mov9toOP1 )
	bCall( _FPDiv )
	call dispOP1		; Display x2
	
	ld hl, OP1
	ld de, FP_x2
	bCall( _Mov9B )
	ld a, tY
	ld (variabletoken), a
	ld hl, FP_x2
	call storeVariable

	bCall( _RunIndicOn )

	ret
	

;; Display OP1
dispOP1:			
	ld a, $09
	bCall( _FormReal )	
	ld hl, OP3		
	bCall( _PutS )		
	bCall( _NewLine )	
	ret

;; Save a Variable to User Memory
storeVariable:
	ld (variabletoken), a
	ld de, variabledata
	ld bc, $09
	ldir
	
	push hl
	push de
	push bc
	ld hl, variablename	; look for the variable in the vat
	bCall( _Mov9toOP1 )	; using its name (FP_VA)
	bCall( _FindSym )	; 
	jr c, storeVAR		; if it isn't found then create it
	bCall( _DelVar )	; and delete it from the VAT
storeVAR:
	ld hl, $09		; Bytes of Memory needed to store it
	bCall( _CreateReal ) 	; de is returned	
	ld hl, variabledata	; Point hl to start of Variable 
	ld bc, $09		; Bytes of Memory needed to store it
	ldir			; copy it to the VAT

	; clear out token
	xor a
	ld (variabletoken), a

	pop bc
	pop de
	pop hl
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
	jp z, readkeyA_decpt
	cp skSub
	jp z, readkeyA_negative
	cp skChs
	jp z, readkeyA_negative
	cp skLeft
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
readkeyA_cr:
;; check to see if this is the first byte-- if it is
;; then do nothing
	ld a, (text_buffer_length) ; if text length = 0 just return
	or a
	jp z, readkeyA0
	
	ld a, $00
	jp readkeyA2
readkeyA_decpt:
	ld a, '.'
	bCall( _PutC )
	ld a, tDecPt
	jp readkeyA2
readkeyA_negative:
	ld a, '-'
	bCall( _PutC )
	ld a, tChs
	jp readkeyA2
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

; DATA SECTION
variablename:
.db RealObj
variabletoken:
.db $00, $00, $00
variabledata:
.db $00, $00, $00, $00, $00, $00, $00, $00, $00

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
.db "> ",0


prompt_text_sum:
.db "Sum",0
prompt_text_prod:
.db "Prod",0
output_none:
.db "No Real Numbers",0

FP_sum:
.db $00, $00, $00, $00, $00, $00, $00, $00, $00
FP_prod:
.db $00, $00, $00, $00, $00, $00, $00, $00, $00
FP_x1:
.db $00, $00, $00, $00, $00, $00, $00, $00, $00
FP_x2:
.db $00, $00, $00, $00, $00, $00, $00, $00, $00
