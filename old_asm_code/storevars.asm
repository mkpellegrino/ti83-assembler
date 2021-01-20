;; storevars
;;
;; graphs a function in TI83 plus assembler
;; based on the Floating Point Number coefficients
;; in FP_A, FP_B, and FP_C.  The coefficients
;; are stored in User Variables A, B, and C
;; and then the Graph of Y1=AX^2+BX+C is drawn.
;;
;; (C) 2017 - Mike Pellegrino
	
#include    "ti83plus.inc"
#define     progStart   $9D95
.org        progStart-2
.db         $BB,$6D
	
	ld a, tA
	ld hl, FP_A
	call storeVariable

	ld hl, FP_B
	ld a, tB
	call storeVariable

	ld hl, FP_C
	ld a, tC
	call storeVariable

	; Now create a String of Tokens Ax^2+Bx+C
	ld hl, equationName
	bCall( _Mov9toOP1 )
	bCall( _FindSym )
	jr c, storeEquation
	push hl			; otherwise display "Deleting Var"
	push de
	ld hl, DelMsg
	bCall( _PutS )
	bCall( _NewLine )
	pop de
	pop hl
	bCall( _DelVar )
storeEquation:
	ld hl, $08
	bCall( _CreateStrng )
	inc de
	inc de
	ld hl, equation
	ld bc, $08
	ldir

	; Graph it
	ld hl, equationName
        bCall( _Mov9toOP1 )
        bCall( _PushRealO1 )
        bCall( _DrawCmd )
        bCall( _PopRealO1 )
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
	bCall( _Mov9toOP1 )	; using its name (FP_VA)
	bCall( _FindSym )	; 
	jr c, storeVAR		; if it isn't found then create it
	
	push hl			; otherwise display "Deleting Var"
	push de
	ld hl, DelMsg
	bCall( _PutS )
	bCall( _NewLine )
	pop de
	pop hl
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
DelMsg:
.db "Deleting Var",0

variablename:
.db RealObj
variabletoken:
.db $00, $00, $00
	
variabledata:
.db $00, $00, $00, $00, $00, $00, $00, $00, $00


FP_A:
.db $80,$7F,$25,$00,$00,$00,$00,$00,$00
FP_B:
.db $00,$80,$20,$00,$00,$00,$00,$00,$00
FP_C:
.db $00,$80,$10,$00,$00,$00,$00,$00,$00

equationName:
.db StrngObj,tVarStrng,tStr1,$00,$00
equation:
.db tA, tX, tSqr, tAdd, tB, tX, tAdd, tC, $00
