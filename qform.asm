.name qform
begin:

clsA:
	call &fully_clear_screen
	ld hl, &title
	bCall(PutS)

	;; PUT THESE IN A LOOP!
	ld hl, &text_promptA
	bCall(PutS)
	call &user_input
	ld hl, &FP_bfr
	ld de, &FP_A
	bCall(Mov9B)
	bCall(NewLine)

	
	ld hl, &text_promptB
	bCall(PutS)
	call &user_input
	ld hl, &FP_bfr
	ld de, &FP_B
	bCall(Mov9B)
	bCall(NewLine)

	ld hl, &text_promptC
	bCall(PutS)
	call &user_input
	ld hl, &FP_bfr
	ld de, &FP_C
	bCall(Mov9B)
	bCall(NewLine)

	;; DOWN TO HERE
	
	ld hl, &FP_A

	bCall(Mov9ToOP1)

	bCall(OP2Set2)
	bCall(FPMult)
	ld hl, &OP1
	ld de, &FP_2A
	bCall(Mov9B)
	
	ld hl, &FP_2A
	bCall(Mov9ToOP2)

	ld hl, &FP_B
	rst 0x0020
	bCall(InvOP1S)
	bCall(FPDiv)
	ld hl, &OP1
	ld de, &FP_AOS
	bCall(Mov9B)
	
	ld hl, &FP_A
	bCall(Mov9ToOP2)
	ld hl, &FP_C
	rst 0x0020
	bCall(FPMult)
	bCall(OP2Set4)
	bCall(FPMult)
	bCall(InvOP1S)

	ld hl, &OP1
	ld de, &FP_bfr
	bCall(Mov9B)
	
	ld hl, &FP_B
	rst 0x0020
	bCall(FPSquare)
	ld hl, &FP_bfr
	bCall(Mov9ToOP2)
	bCall(FPAdd)

	ld hl, &OP1
	ld de, &FP_Discriminant
	bCall(Mov9B)
	
	bCall(ZeroOP2)
	bCall(CpOP1OP2)

	jr c, %complex_solutions
	
	bCall(Sqroot)
	ld hl, &OP1
	ld de, &FP_Discriminant
	bCall(Mov9B)

	ld hl, &FP_2A
	bCall(Mov9ToOP2)
	bCall(FPDiv)

	ld hl, &OP1
	ld de, &FP_tmp
	bCall(Mov9B)

	ld hl, &FP_AOS
	rst 0x0020

	ld hl, &FP_tmp
	bCall(Mov9ToOP2)

	bCall(FPAdd)
	
	ld hl, &OP1
	ld de, &FP_Solution1
	bCall(Mov9B)

	call &disp_op1

	ld hl, &FP_AOS
	rst 0x0020

	ld hl, &FP_tmp
	bCall(Mov9ToOP2)

	bCall(FPSub)
	
	ld hl, &OP1
	ld de, &FP_Solution2
	bCall(Mov9B)

	call &disp_op1

	ld hl, &FP_Solution1
	ld a, tX
	call storeVariable

	ld hl, &FP_Solution1
	ld a, tAns
	call storeVariable
	
	ld hl, &FP_Solution2
	ld a, tY
	call storeVariable

	
	ret

	
complex_solutions:
	
	ld hl, &FP_Discriminant
	rst 0x0020
	xor a
	ld (&OP1), a
	bCall(SqRoot)
	ld hl, &FP_2A
	bCall(Mov9ToOP2)
	bCall(FPDiv)
	ld hl, &OP1
	ld de, &FP_S1C
	bCall(Mov9B)

	ld hl, &FP_AOS
	ld de, &FP_S1R
	bCall(Mov9B)
	
	ld hl, &FP_S1R
	ld a, (hl)
	or @0x0C
	ld (hl), a

	ld hl, &FP_S1C
	ld a, @0x0C
	ld (hl), a
	
	ld hl, &FP_Solution1
	ld a, tX
	call &storeVariableC

	ld hl, &FP_Solution1
	ld a, tAns
	call storeVariable
	
	ld hl, &FP_S1R
	ld de, &FP_S2R
	bCall(Mov9B)

	ld hl, &FP_S1C
	ld de, &FP_S2C
	bCall(Mov9B)

	ld hl, &FP_S2C
	ld a, @0x8C
	ld (hl), a

	ld hl, &FP_Solution2
	ld a, tY
	call storeVariableC

	ld hl, &FP_S1R
	rst 0x0020
	ld hl, &FP_S1C
	bCall(Mov9ToOP2)
	bCall(FormDCplx)
	ld hl, &fmtString
	bCall(PutS)
	bCall(NewLine)
	ld hl, &FP_S2R
	rst 0x0020
	ld hl, &FP_S2C
	bCall(Mov9ToOP2)
	bCall(FormDCplx)
	ld hl, &fmtString
	bCall(PutS)
	bCall(NewLine)
	
	ret

variablenameC:
.db CplxObj
variabletokenC:
.db 0x00
.db 0x00
.db 0x00
variabledataC:
.fp variabledataC
.fp variabledataC-PartII
	
	
title:
;;  1234567890123456
.str "   Quadratic     Formula Solver ================by mr pellegrino================",0

.fp FP_A
.fp FP_B
.fp FP_C
.fp FP_Discriminant
.fp FP_AOS
.fp FP_tmp
.fp FP_2A

	
;; These are twice as big as FP's because they could be complex
FP_S1R:
.fp FP_Solution1
.fp FP_S1C
FP_S2R:
.fp FP_Solution2
.fp FP_S2C
	
.fp FP_bfr
FP_bfr:
.db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00

generic_prompt:
.str "> "
text_promptA:
.str "A"
text_promptB:
.str "B"
text_promptC:
.str "C"
