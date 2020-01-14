;; noformat.asm
;; This TI-83 Assembler program (z80)
;; takes a float that is < 0.001 and creates
;; a string that is: 0.000...
;; this can be used to create a graph with
;; numbers of small magnitude
	
#include    "ti83plus.inc"
#define     progStart   $9D95
.org        progStart-2
.db         $BB,$6D

begin:
	ld hl, FP_A
	inc hl
	ld a, (hl) 		; _GetExp
	
	ld b, a
	ld a, $80
	sub b
	ld (FP_num_zer), a
	or $80

	ld hl, FP_B
	inc hl
	ld (hl), a
	
	ld hl, FP_B		; OP2
	bCall( _Mov9toOP2 )
	
	ld hl, FP_A
	bCall( _Mov9toOP1 )

	bCall( _FPMult )
	
	ld a, $0A
	bCall( _FormReal )

	ld hl, FP_A 		; Check for FP_A<0 and store a - if it is
	ld a, (hl)
	cp $80
	ld hl, Txt_bfr
	jr nz, positive
	ld (hl), '-'
	inc hl
positive:
	ld (hl), $30
	inc hl
	ld (hl), '.'
	inc hl

	; Now put the correct number of zeroes
	ld a, (FP_num_zer)
	ld b, a
positive0:
	ld (hl), $30
	inc hl
	djnz positive0
	ex de, hl
	ld hl, OP3
	ld a, (hl)
	cp '-'
	jr nz, positive1
	inc hl
positive1:
	inc hl
	inc hl			; skip leading 0
	inc hl			; skip decimal point
	ld bc, $000E
	ldir

	ld (hl), $00
		
	ld hl, Txt_bfr
	bCall( _PutS )
	ret

FP_num_zer:
.db $00
	
FP_A:
.db $00,$7C,$25,$12,$70,$00,$00,$00,$00,$00,$00,$00 ; -0.0000025

FP_B:
.db $00,$00,$01,$00,$00,$00,$00,$00,$00,$00,$00,$00 ; 1000000

Txt_bfr:
.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
;     1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19  20  21 
