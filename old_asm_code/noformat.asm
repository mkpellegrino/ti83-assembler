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

flt2str:
	push hl
	push de
	push af
	push bc

	ld hl, flt2str_bfr3	; clear out the string buffer
	ld b, $15
	xor a
	ld (FP_num_zer), a	; clear out this variable while we're at it
flt2str0p1:
	ld (hl), a
	inc hl
	djnz flt2str0p1
	
	ld hl, flt2str_bfr
	inc hl
	ld a, (hl) 		; _GetExp
	
	ld b, a
	ld a, $80
	sub b
	ld (FP_num_zer), a
	or $80

	ld hl, flt2str_bfr2
	inc hl
	ld (hl), a
	
	ld hl, flt2str_bfr2		; OP2
	bCall( _Mov9toOP2 )
	
	ld hl, flt2str_bfr
	bCall( _Mov9toOP1 )

	bCall( _FPMult )
	
	ld a, $0A
	bCall( _FormReal )

	ld hl, flt2str_bfr 		; Check for flt2str_bfr<0 and store a - if it is
	ld a, (hl)
	cp $80
	ld hl, flt2str_bfr3
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
		
	;ld hl, flt2str_bfr3
	;bCall( _PutS )

	ld hl, flt2str_bfr
	xor a
	ld b, $0C
positive2:
	ld (hl), a
	inc hl
	djnz positive2
	
	pop bc
	pop af
	pop de
	pop hl
	ret

FP_num_zer:
.db $00
	
flt2str_bfr:
.db $00,$7C,$25,$12,$70,$00,$00,$00,$00,$00,$00,$00
flt2str_bfr2:
.db $00,$00,$01,$00,$00,$00,$00,$00,$00,$00,$00,$00
flt2str_bfr3:
.db $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
