.name looptst3
        bCall(RunIndicOff)
	call &fully_clear_screen
	ld hl, &prompt1
	bCall(PutS)

;;; store the location of the 1st FP in index
	ld hl, &FP_1
	ld (&index), hl

;;; store the location of the 1st prompt in prompt_i
	ld hl, &prompt2
	ld (&prompt_index), hl

	
	ld hl, &loopA
	push hl
	ld hl, #0x0004
	push hl
	loop

;;; reset the fp index
;;; (store the location of the 1st FP in index)
	ld hl, &FP_1
	ld (&index), hl


	ld hl, &loopB
	push hl
	ld hl, #0x0004
	push hl
	loop
	
	bCall(RunIndicOn)
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

loopB:
	pusha
	ld hl, (&index)

 	bCall(Mov9ToOP1)
 	call &disp_op1

;;; now increase the index by 9 bytes
	ld hl, (&index)	
	ld bc, #0x0009
 	add hl, bc
	ld (&index), hl
	
	popa
	ret

prompt1:
.str "Enter 4 numbers:"

index:
.dw 0x0000
.fp FP_1
.fp FP_2
.fp FP_3
.fp FP_4

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
