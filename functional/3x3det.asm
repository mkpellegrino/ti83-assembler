.name determ
	ld hl, &title
	bCall(PutS)
	ld hl, &FP_a
	ld (&index), hl
	ld hl, &loop_inputs
	push hl
	ld hl, #0x0009
	push hl
	loop
	ld hl, &FP_g
	bCall(Mov9ToOP2)
	ld hl, &FP_e
	bCall(Mov9ToOP1)
	bCall(FPMult)
	ld de, &OP6
	bCall(MovFrOP1)
	ld hl, &FP_h
	bCall(Mov9ToOP2)
	ld hl, &FP_d
	bCall(Mov9ToOP1)
	bCall(FPMult)
	ld hl, &OP6
	bCall(Mov9ToOP2)
	bCall(FPSub)
	ld hl, &FP_c
	bCall(Mov9ToOP2)
	bCall(FPMult)
	ld de, &OP4
	bCall(MovFrOP1)
	ld hl, &FP_g
	bCall(Mov9ToOP2)
	ld hl, &FP_f
	bCall(Mov9ToOP1)
	bCall(FPMult)
	ld de, &OP6
	bCall(MovFrOP1)
	ld hl, &FP_i
	bCall(Mov9ToOP2)
	ld hl, &FP_d
	bCall(Mov9ToOP1)
	bCall(FPMult)
	ld hl, &OP6
	bCall(Mov9ToOP2)	
	bCall(FPSub)
	ld hl, &FP_b
	bCall(Mov9ToOP2)
	bCall(FPMult)
	ld de, &OP5
	bCall(MovFrOP1)
	ld hl, &FP_h
	bCall(Mov9ToOP2)
	ld hl, &FP_f
	bCall(Mov9ToOP1)
	bCall(FPMult)
	ld de, &OP6
	bCall(MovFrOP1)
	ld hl, &FP_i
	bCall(Mov9ToOP2)
	ld hl, &FP_e
	bCall(Mov9ToOP1)
	bCall(FPMult)
	ld hl, &OP6
	bCall(Mov9ToOP2)
	bCall(FPSub)
	ld hl, &FP_a
	bCall(Mov9ToOP2)
	bCall(FPMult)
	ld hl, &OP5
	bCall(Mov9ToOP2)
	bCall(FPSub)
	ld hl, &OP4
	bCall(Mov9ToOP2)
	bCall(FPAdd)
 	call &disp_op1
	bCall(StoAns)
	ret
loop_inputs:
	pusha
	ld hl, &prompt_a
	bCall(PutS)
	ld hl, &prompt_a
	ld a, (hl)
	inc a
	ld (hl), a
	call &user_input
	ld de, (&index)
	bCall(MovFrOP1)
	ld hl, (&index)
	ld bc, #0x0009
	add hl, bc
	ld (&index), hl
	popa	
	ret

title:
;;;   123456789ABCDEF-123456789ABCDEF-123456789ABCDEF-
.str "3x3 matrix det  by mr pellegrino----------------"

index:
.dw 0x0000
.fp FP_a
.fp FP_b
.fp FP_c
.fp FP_d
.fp FP_e
.fp FP_f
.fp FP_g
.fp FP_h
.fp FP_i

prompt_a:
.str "a> "
