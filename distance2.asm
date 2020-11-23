.name distance
;;; bCall(RunIndicOff)
;;; call &fully_clear_screen
	ld hl, &prompt1
	bCall(PutS)

;;; store the location of the 1st FP in index
	ld hl, &FP_x1
	ld (&index), hl

;;; store the location of the 1st prompt in prompt_i
	ld hl, &prompt2
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
	bCall(Mov9ToOP2)
	
	bCall(FPAdd)
	bCall(SqRoot)

	ld de, &FP_distance
	bCall(MovFrOP1)

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
;;; bCall(NewLine)


	
;;; 	bCall(RunIndicOn)
	ret
loopA:
	pusha

	ld hl, (&prompt_index)
	bCall(PutS)

;;; point the prompt_i to the next prompt 
	ld hl, (&prompt_index)
	ld bc, #0x0005
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
	popa
	
	ret

prompt1:
.str "Distance Formulaby mr pellegrino----------------"

index:
.dw 0x0000
FP_left:	
.fp FP_x1
FP_right:
.fp FP_y1
FP_distance:
.fp FP_x2
.fp FP_y2

prompt_index:
.dw 0x0000
prompt2:
.str "X1> "
prompt3:
.str "Y1> "
prompt4:
.str "X2> "
prompt5:
.str "Y2> "

text_distance:
.str "Dst: "
