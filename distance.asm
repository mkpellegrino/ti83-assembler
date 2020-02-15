;; Mike Pellegrino
;; (C) 2020
;; TI8x Assembler for tias
;; calculates the distance between 2 points
;; distance formula 
.name distance
begin:
	bCall(RunIndicOff)
	call &fully_clear_screen

	ld hl, &prompt_text_x1
	ld (&index), hl
	
	ld hl, &title
	bCall(PutS)

	ld hl, &FP_x1
	ld b, @0x04

loop_top:
	call &show_prompt
	call &user_input
	ld d, h
	ld e, l
	push bc
	push hl
	ld hl, &FP_user_input
	bCall(Mov9B)
	pop hl
	pop bc
	ld de, #0x0009
	add hl, de
	djnz %loop_top

	
	ld hl, &FP_y2
	bCall(Mov9ToOP1)

	ld hl, &FP_y1
	bCall(Mov9ToOP2)

	bCall(FPSub)
	bCall(FPSquare)
	

	ld hl, &OP1
	ld de, &FP_left
	bCall(Mov9B)
	

	ld hl, &FP_x2
	bCall(Mov9ToOP1)

	ld hl, &FP_x1
	bCall(Mov9ToOP2)

	bCall(FPSub)
	bCall(FPSquare)
	
	ld hl, &OP1
	ld de, &FP_right
	bCall(Mov9B)
	

	ld hl, &FP_left
	bCall(Mov9ToOP1)
	
	ld hl, &FP_right
	bCall(Mov9ToOP2)
	
	bCall(FPAdd)
	bCall(SqRoot)

	ld hl, &OP1
	ld de, &FP_distance
	bCall(Mov9B)

	ld a, @0x44
	ld (&variabletoken), a
	ld hl, &FP_distance
	call &store_op1

	ld hl, &text_distance
	bCall(PutS)
	ld hl, &FP_distance
	bCall(Mov9ToOP1)
	bCall(StoAns)

	
	ld a, @0x0A
	bCall(FormReal)	
	ld hl, &OP3		
	bCall(PutS)		
	bCall(NewLine)
	ret

	;show prompt
show_prompt:
	push af
	push bc
	push de
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
	pop de
	pop bc
	pop af
	ret


index:
.dw 0x0000
	
prompt_text_x1:
.str "X1>"
	
prompt_text_y1:
.str "Y1>"
	
prompt_text_x2:
.str "X2>"
	
prompt_text_y2:
.str "Y2>"

.fp FP_x1
.fp FP_y1
.fp FP_x2
.fp FP_y2
.fp FP_left
.fp FP_right

.fp FP_distance	
	
text_distance:
.str "Dst: "

title:
.str "Distance Formulaby mr pellegrino----------------"
