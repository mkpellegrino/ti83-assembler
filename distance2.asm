.name distance
	ld hl, &title
	bCall(PutS)

;;; store the location of the 1st FP in index
	ld hl, &FP_x1
	ld (&index), hl

;;; store the location of the 1st prompt in prompt_i
	ld hl, &promptX1
	ld (&prompt_index), hl
	
	ld hl, &loopA
	push hl
	ld hl, #0x0004
	push hl
	loop

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
	
	ld de, &FP_right
	bCall(MovFrOP1)
	
	ld hl, &FP_left
	bCall(Mov9ToOP1)

	ld hl, &FP_right
	bCall(Mov9ToOP2)
	
	bCall(FPAdd)
	bCall(SqRoot)

 	bCall(StoAns)

	;; Display OP1
   	ld a, @0x0A
 	bCall(FormReal)	
   	ld hl, &OP3		
   	bCall(PutS)		
	
	ret
loopA:
 	push hl
 	push bc
 	push de
	
	ld hl, (&prompt_index)
	bCall(PutS)

;;; point the prompt_i to the next prompt 
	ld hl, (&prompt_index)
	ld bc, #0x0004
	add hl, bc
	ld (&prompt_index), hl
	
	call &user_input
	ld de, (&index)
	bCall(MovFrOP1)
	
;;; now increase the index by 9 bytes
	ld hl, (&index)
	ld bc, #0x0009
	add hl, bc
	ld (&index), hl
 	pop de
 	pop bc
 	pop hl
	ret

title:
.str "Distance Formulaby mr pellegrino----------------"

index:
.dw 0x0000
FP_right:
.fp FP_x1
.fp FP_y1
.fp FP_x2
FP_left:	
.fp FP_y2
	
prompt_index:
.dw 0x0000
promptX1:
.str "X1>"
promptY1:
.str "Y1>"
promptX2:
.str "X2>"
promptY2:
.str "Y2>"
