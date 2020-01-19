;; Quadratic Formula
#include    "ti83plus.inc"
#define     progStart   $9D95
.org        progStart-2
.db         $BB,$6D

begin:
	ld b, $0A

clsA:
	bCall(_NewLine)
	djnz clsA
	xor a	
	ld (CurCol),a
	ld (CurRow),a
	bCall(_ClrLCDFull )

	ld hl, title 		; Display the Title Screen
	bCall( _PutS )		; 

	ld hl, text_promptA	;prompt
	bCall( _PutS )		;display

	call getuserinput	;input

	ld hl, FP_bfr		; load source address
	ld de, FP_A		; load dest address
	call Store9Bytes	; copy 9 bytes src->dest

	bCall( _NewLine )	; crlf

	;ld a, tA
	;ld hl, FP_A
	;call storeVariable
	
	ld hl, text_promptB	;prompt
	bCall( _PutS )		;display

	call getuserinput	;input

	ld hl, FP_bfr		; load source address
	ld de, FP_B		; load dest address
	call Store9Bytes	; copy 9 bytes src->dest

	bCall( _NewLine )	; crlf

	;ld a, tB
	;ld hl, FP_B
	;call storeVariable

	ld hl, text_promptC	;prompt
	bCall( _PutS )		;display
	call getuserinput	;input
	ld hl, FP_bfr		; load source address
	ld de, FP_C		; load dest address
	call Store9Bytes	; copy 9 bytes src->dest
	bCall( _NewLine )	; crlf

	;ld a, tC
	;ld hl, FP_C
	;call storeVariable

;;; Find 2A and Store it
	ld hl, FP_A

	rst $0020
	;bCall( _Mov9toOP1 )

	bCall( _OP2Set2 )
	bCall( _FPMult )
	ld hl, OP1
	ld de, FP_2A
	call Store9Bytes
	
;;; Find AOS
	ld hl, FP_2A
	bCall( _Mov9toOP2 ) 	; A->OP1

	ld hl, FP_B
	rst $0020
	;bCall( _Mov9toOP1 )	; B->OP1
	bCall( _InvOP1S )	; -B->OP1
	bCall( _FPDiv )		; -B/2A -> OP1
	ld hl, OP1
	ld de, FP_AOS
	call Store9Bytes
	
;;; Find discriminant
	ld hl, FP_A
	bCall( _Mov9toOP2 )	; A->OP2
	ld hl, FP_C
	rst $0020
	;bCall( _Mov9toOP1 )	; C->OP1
	bCall( _FPMult )	; AC->OP1
	bCall( _OP2Set4 )	; 4->OP2
	bCall( _FPMult )	; 4AC->OP1
	bCall( _InvOP1S )	; -4AC->OP1

	ld hl, OP1
	ld de, FP_bfr
	call Store9Bytes
	
	ld hl, FP_B
	rst $0020
	;bCall( _Mov9toOP1 )	; B->OP1
	bCall( _FPSquare )	; B^2 ->OP1
	ld hl, FP_bfr
	bCall( _Mov9toOP2 )
	bCall( _FPAdd )		; B^2 - 4AC -> OP1 :  (OP1 + OP2)-> OP1

	ld hl, OP1
	ld de, FP_Discriminant
	call Store9Bytes	; Store the value of the Discriminant in RAM
	
	bCall( _ZeroOP2 )	; 0->OP2
	bCall( _CpOP1OP2 )	; Compare Discriminant with 0

	jr c, complex_solutions	; on carry OP1 < OP2 : (OP1<0) so solutions are complex
;;; stuff here for solutions in R

	
	bCall( _Sqroot )
	ld hl, OP1
	ld de, FP_Discriminant
	call Store9Bytes	; Store the value of the Discriminant in RAM

	ld hl, FP_2A
	bCall( _Mov9toOP2 )
	bCall( _FPDiv )

	ld hl, OP1
	ld de, FP_tmp
	call Store9Bytes

	ld hl, FP_AOS
	rst $0020
	;bCall( _Mov9toOP1 )

	ld hl, FP_tmp
	bCall( _Mov9toOP2 )

	bCall( _FPAdd )
	
	ld hl, OP1
	ld de, FP_Solution1
	call Store9Bytes

	call dispOP1

	ld hl, FP_AOS
	rst $0020
	;bCall( _Mov9toOP1 )

	ld hl, FP_tmp
	bCall( _Mov9toOP2 )

	bCall( _FPSub )
	
	ld hl, OP1
	ld de, FP_Solution2
	call Store9Bytes

	call dispOP1

	ld hl, FP_Solution1
	ld a, tX
	call storeVariable
	ld hl, FP_Solution2
	ld a, tY
	call storeVariable

	ret

	
complex_solutions:		; 
	
	ld hl, FP_Discriminant
	rst $0020
	;bCall( _Mov9toOP1 )
	xor a
	ld (OP1), a		; Abs(OP1)	
	bCall( _Sqroot )
	ld hl, FP_2A
	bCall( _Mov9toOP2 )
	bCall( _FPDiv )
	ld hl, OP1
	ld de, FP_S1C
	call Store9Bytes

	ld hl, FP_AOS
	ld de, FP_S1R
	call Store9Bytes
	
	ld hl, FP_S1R
	ld a, (hl)
	or $0C
	ld (hl), a

	ld hl, FP_S1C
	ld a, $0C
	ld (hl), a
	
	ld hl, FP_Solution1
	ld a, tX
	call storeVariableC

	ld hl, FP_S1R
	ld de, FP_S2R
	call Store9Bytes

	ld hl, FP_S1C
	ld de, FP_S2C
	call Store9Bytes

	ld hl, FP_S2C
	ld a, $8C
	ld (hl), a

	ld hl, FP_Solution2
	ld a, tY
	call storeVariableC

	ld hl, FP_S1R
	rst $0020
	;bCall( _Mov9toOP1 )
	ld hl, FP_S1C
	bCall( _Mov9toOP2 )
	bCall( _FormDCplx )
	ld hl, fmtString
	bCall( _PutS )
	bCall( _NewLine )
	ld hl, FP_S2R
	rst $0020
	;bCall( _Mov9toOP1 )
	ld hl, FP_S2C
	bCall( _Mov9toOP2 )
	bCall( _FormDCplx )
	ld hl, fmtString
	bCall( _PutS )
	bCall( _NewLine )
	
	ret
	
dispOP1:			
	ld a, $09
	bCall( _FormReal )	
	ld hl, OP3		
	bCall( _PutS )		
	bCall( _NewLine )	
	ret

getuserinput:
	push hl
	push af
	push bc
	push de

	xor a
	ld (text_buffer_length), a
	
	ld hl, text_buffer
	ld (text_buffer_ptr), hl
	
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
	rst $0020
	;bCall( _Mov9toOP1 )	; using its name (FP_VA)
	bCall( _FindSym )	; 
	bCall( _DelVar )	; and delete it from the VAT

	pop de
	pop bc
	pop af
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
	

;;; FUNCTION Store9Bytes
Store9Bytes:			; DE = Dest Addr, HL = Src Addr
	ld (store9_hl), hl
	ex de, hl
	ld (store9_de), hl
	push hl
	push de
	push bc
	ld hl, (store9_hl)
	ld de, (store9_de)
	ld bc, $09
	ldir
	pop bc
	pop hl			; Switched the order here because I did an
	pop de			; exchange at the beginning of the func.
	ret
		
storeVariable:
	ld (variabletoken), a
	ld de, variabledata
	ld bc, $09
	ldir

	push hl
	push de
	push bc
	ld hl, variablename	; look for the variable in the vat
	rst $0020
	;bCall( _Mov9toOP1 )	; using its name (FP_VA)
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



storeVariableC:
	ld (variabletokenC), a
	ld de, variabledataC
	ld bc, $12
	ldir

	push hl
	push de
	push bc
	ld hl, variablenameC	; look for the variable in the vat
	bCall( _Mov9toOP1 )	; using its name (FP_VA)
	bCall( _FindSym )	; 
	jr c, storeVARC		; if it isn't found then create it
	bCall( _DelVar )	; and delete it from the VAT
storeVARC:
	ld hl, $12		; Bytes of Memory needed to store it
	bCall( _CreateCplx ) 	; de is returned	
	ld hl, variabledataC	; Point hl to start of Variable 
	ld bc, $12		; Bytes of Memory needed to store it
	ldir			; copy it to the VAT

	; clear out token
	xor a
	ld (variabletokenC), a

	pop bc
	pop de
	pop hl
	ret


create_equation:
	ld hl, equationName	; look for the equation in the vat
	bCall( _Mov9toOP1 )	; using its name (variablename)
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


; DATA SECTION
variablename:
.db RealObj
variabletoken:
.db $00, $00, $00
variabledata:
.db $00, $00, $00, $00, $00, $00, $00, $00, $00


variablenameC:
.db CplxObj
variabletokenC:
.db $00, $00, $00
variabledataC:
.db $00, $00, $00, $00, $00, $00, $00, $00, $00
.db $00, $00, $00, $00, $00, $00, $00, $00, $00
	
	
title:
;;   1234567890123456
.db "   Quadratic    "
.db " Formula Solver "
.db "================"
.db "by mr pellegrino"
.db "================",0

FP_A:
.db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
FP_B:
.db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
FP_C:
.db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00

FP_Discriminant:
.db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00

FP_AOS:
.db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00


FP_tmp:
.db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00

FP_2A:
.db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
	
;; These are twice as big as FP's because they could be complex
FP_S1R:				;Floating Point Solution 1 Real Portion
FP_Solution1:
.db $00, $00, $00, $00, $00, $00, $00, $00, $00
FP_S1C:				;Floating Point Solution 1 Imaginary Portion
.db $00, $00, $00, $00, $00, $00, $00, $00, $00
FP_S2R:				;Floating Point Solution 2 Real Portion
FP_Solution2:			
.db $00, $00, $00, $00, $00, $00, $00, $00, $00
FP_S2C:				;Floating Point Solution 2 Imaginary Portion
.db $00, $00, $00, $00, $00, $00, $00, $00, $00

	
store9_hl:
.dw $0000
store9_de:
.dw $0000

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

text_promptA:
.db "A: ",0
text_promptB:
.db "B: ",0
text_promptC:
.db "C: ",0
