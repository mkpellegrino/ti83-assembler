;; graph.asm
;; graphs a function in TI83 plus assembler
	
#include    "ti83plus.inc"
#define     progStart   $9D95
.org        progStart-2
.db         $BB,$6D

storeeq:
	set graphDraw, (IY + graphFlags)
	set smartGraph_inv, (IY + smartFlags)
	
	ld hl, FP_zero ; Window Settings
	ld de, Xmin
	ld bc, $09
	ldir
	
	ld hl, FP_zero
	ld de, Ymin
	ld bc, $09
	ldir
	
	ld hl, FP_10
	ld de, Xmax
	ld bc, $09
	ldir
	
	ld hl, FP_250
	ld de, Ymax
	ld bc, $09
	ldir
	
	;set graphDraw, (IY + graphFlags)
	ld hl, FP_S1		; Name FP
	bCall( _Mov9toOP1 )	; Move it to OP1
	bCall( _FindSym )	; Look for it in VAT
	
	jr c, storeeq1 		;it's not found so skip deleting it
	push hl
	push de
	ld hl, DelMsg
	bCall( _PutS )
	bCall( _NewLine )
	pop de
	pop hl
	bCall( _DelVar )
storeeq1:
	
	ld hl, FP_S1		; Create the string
	bCall( _Mov9toOP1 )
	ld hl, $11
	bCall( _CreateStrng ) 	; de is returned	
	inc de			; de=start of data in mem
	inc de			; so inc inc to move it
	ld hl, FP_S2_data	; past the data size
	ld bc, $0F
	ldir

	ld hl, FP_S1
	bCall( _Mov9toOP1 )
	bCall( _PushRealO1 )
	bCall( _DrawCmd )
	bCall( _PopRealO1 )
	ret
DelMsg:
.db "Deleting Var",0
FP_S1:
.db StrngObj,tVarStrng,tStr1,0,0
FP_S1_data:
.db t4, tSub, t1, t6, tX, tSqr, tSpace, tSpace, tSpace
FP_zero:
.db $00,$80,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
FP_250:
.db $00,$82,$25,$00,$00,$00,$00,$00,$00,$00,$00,$00
FP_10:
.db $00,$81,$10,$00,$00,$00,$00,$00,$00,$00,$00,$00

FP_S2_data:
.db t4, t0, tX, tAdd, t2, t2, t5, tSub, t1, t6, tDecPt, t0, t9, tX, tSqr
