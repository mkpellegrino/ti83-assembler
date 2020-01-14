;; appvar.asm
;;
;; A TI-83+ Assembly (z80) program that
;; stores a "hidden" AppVar loaded with OP1
;; and then recalls it back into OP1
;;
;; (C) 2017 - Michael K. Pellegrino 				;

#include    "ti83plus.inc"
#define     progStart   $9D95
.org        progStart-2
.db         $BB,$6D

;;; STORE THE APP VAR: " BJX" in MEMORY AS FP 1.5
	bCall( _OP1Set3 )
	bCall( _TimesPt5 )
	ld hl, OP1
	ld a, tX
	call storeAppVar

;;; RECALL THE APP VAR: " BJX" from MEMORY
	ld a, tX
	call recallAppVar
	call dispOP1
end:
	;; Clean Up
	ld a, tX
	ld (appvarname), a
	ld hl, appvar	; look for the variable in the vat
	rst $0020	; Mov9toOP1
	bCall( _ChkFindSym )	
	bCall( _DelVar )
	ret
	
dispOP1:
	push hl
	push af
	ld a, $06
	bCall( _FormReal )	
	ld hl, OP3		
	bCall( _PutS )		
	bCall( _NewLine )
	pop af
	pop hl
	ret
	
recallAppVar:
	ld (appvarname), a
	push hl
	push de
	push bc
	ld hl, appvar
	rst $0020
	bCall( _ChkFindSym )
	jr c, recallAppVar1
	ex de, hl
	inc hl
	inc hl
	ld de, appvardata
	ld bc, $0009
	ldir
	ld hl, appvardata
	rst $0020
recallAppVar1:
	pop bc
	pop de
	pop hl

	ret
	
storeAppVar:
	ld (appvarname), a
	ld de, appvardata
	ld bc, $09
	ldir
	
	push hl
	push de
	push bc
	ld hl, appvar		; look for the variable in the vat
	rst $0020	
	bCall( _ChkFindSym )	; 
	jr c, storeVAR		; if it isn't found then create it
	bCall( _DelVar )	; and delete it from the VAT
storeVAR:
	ld hl, appvar		; look for the variable in the vat
	rst $0020
	ld hl, $000B		; Bytes of Memory needed to store data plus a 2 bytes size
	bCall( _CreateAppVar ) 	; de is returned
	ld hl, appvardata	; Point hl to start of Variable
	inc de
	inc de
	ld bc, $0009		; Bytes of Memory needed to store data
	ldir			; copy it to the VAT

	; clear out token
	xor a
	ld (appvarname), a

	pop bc
	pop de
	pop hl
	ret
appvar:
.db AppVarObj
.db tSpace
.db tB, tJ
appvarname:
.db $00, $00, $00, $00, $00, $00
appvardata:
.db $00, $00, $00, $00, $00, $00, $00, $00, $00
