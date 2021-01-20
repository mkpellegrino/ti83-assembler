;; Mike Pellegrino
;; (C) 2020
;; TI8x Assembler for tias
;; calculates the distance between 2 points
;; distance formula 
.name distance
begin:
	ld hl, &prompt_text_x1
	ld (&index), hl

	ld hl, &FP_x1
	ld b, @0x04

loop_top:
 	push hl	
	ld hl, (&index)
 	push hl
	bCall(PutS)
 	pop hl
 	inc hl
 	inc hl
 	inc hl
 	inc hl
 	ld (&index), hl
 	pop hl
	
	call &user_input
	ld de, hl
	push hl
	ld hl, &FP_user_input
	bCall(MovFrOP1)
	pop hl
	ld de, #0x0009
	add hl, de
	djnz %loop_top

	ld hl, &FP_y2
	bCall(Mov9ToOP1)

	ld hl, &FP_y1
	bCall(Mov9ToOP2)

	bCall(FPSub)
	bCall(FPSquare)
	
	ld de, &FP_left
	bCall(MovFrOP1)

	ld hl, &FP_x2
	bCall(Mov9ToOP1)

	ld hl, &FP_x1
	bCall(Mov9ToOP2)

	bCall(FPSub)
	bCall(FPSquare)
	
	ld hl, &FP_left
	bCall(Mov9ToOP2)
	
	bCall(FPAdd)
	bCall(SqRoot)

	bCall(StoAns)
	
	ld a, @0x0A
	bCall(FormReal)	
	ld hl, &OP3		
	bCall(PutS)		
	ret
title:
;;; .str "Distance Formulaby mr pellegrino----------------"

index:
.dw 0x0000
.fp FP_x1
.fp FP_y1
.fp FP_x2
FP_left:	
.fp FP_y2
	
prompt_index:
prompt_text_x1:
.str "X1>"
prompt_text_y1:
.str "Y1>"	
prompt_text_x2:
.str "X2>"
prompt_text_y2:
.str "Y2>"
