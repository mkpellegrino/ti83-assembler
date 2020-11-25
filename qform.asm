.name quadform
start:
	bCall(RunIndicOff)
	call &fully_clear_screen
	ld hl, &title
	bCall(PutS)

;; Get 3 Inputs (b=3)
;; A, B, and C  ('A' is 0x41)
;; Because there memory locations
;; are all 9 apart, we can
;; just keep adding 0x0009 to
;; HL to fill them up
;; The first one is at: &FP_A
	
	ld a, @0x41
	ld hl, &FP_A
	ld b, @0x03
loop_top:
	bCall(PutC)
	push af
	ld a, @0x3E
	bCall(PutC)
	pop af

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
	inc a
	djnz %loop_top

	
	
;;; Calculate 2*A
	ld hl, &FP_A
	bCall(Mov9ToOP1)
	bCall(OP2Set2)
	bCall(FPMult)

;;; 	ld hl, &OP1
	ld de, &FP_2A
;;;     bCall(Mov9B)
	bCall(MovFrOP1)
	
;;; Calculate -4AC
	ld hl, &FP_A
	bCall(Mov9ToOP1)

	ld hl, &FP_C
	bCall(Mov9ToOP2)

	bCall(FPMult)
	bCall(OP2Set4)
	bCall(FPMult)

;;; 	ld hl, &OP1
	ld de, &FP_4AC
;;; 	bCall(Mov9B)
	bCall(MovFrOP1)

;;; Calculate B^2
	ld hl, &FP_B
	bCall(Mov9ToOP1)
	bCall(FPSquare)

;;; Calculate B^2-4AC
	ld hl, &FP_4AC
	bCall(Mov9ToOP2)
	bCall(FPSub)
	
;; If Discriminant is < 0 then the results
;; will be in the Complex Plane
;;; 	ld hl, &OP1
	ld de, &FP_Discriminant
;;; 	bCall(Mov9B)
	bCall(MovFrOP1)
	
	bCall(ZeroOP2)
	bCall(CpOP1OP2)

	jp c, &complex_solutions
;; otherwise, just calculate them and store them in X and Y
	bCall(SqRoot)
	bCall(OP1ToOP2)
	
	ld hl, &FP_B
	bCall(Mov9ToOP1)
	bCall(InvOP1S)
	
	bCall(FPAdd)

	ld hl, &FP_2A
	bCall(Mov9ToOP2)
	bCall(FPDiv)
;; ***********************88*************
	; this is one solution
	bCall(StoX)
	
;;; 	ld hl, &OP1
	ld de, &FP_Solution1
;;; 	bCall(Mov9B)
	bCall(MovFrOP1)
	
	;;Do it again here but with FPSub
	ld hl, &FP_Discriminant
	bCall(Mov9ToOP1)
	bCall(SqRoot)
	bCall(OP1ToOP2)

	ld hl, &FP_B
	bCall(Mov9ToOP1)
	bCall(InvOP1S)
	
	bCall(FPSub)

	ld hl, &FP_2A
	bCall(Mov9ToOP2)
	bCall(FPDiv)
	
	bCall(StoY)

;;; 	ld hl, &OP1
	ld de, &FP_Solution2
;;; 	bCall(Mov9B)
	bCall(MovFrOP1)

show_solutions:
	; This will show both solutions
	; be they real or complex
	ld hl, &FP_Solution2
	bCall(Mov9ToOP1)
	bCall(StoY)

	ld hl, &FP_S1R
	bCall(Mov9ToOP1)
	ld hl, &FP_S1C
	bCall(Mov9ToOP2)
	bCall(FormDCplx)
	ld hl, #0x97B1
	bCall(PutS)
	bCall(NewLine)
	
	ld hl, &FP_S2R
	bCall(Mov9ToOP1)
	ld hl, &FP_S2C
	bCall(Mov9ToOP2)
	bCall(FormDCplx)
	ld hl, #0x97B1
	bCall(PutS)
	bCall(NewLine)

	ret

	
complex_solutions:

	; Calculate the AOS (Again!)
	ld hl, &FP_B
	bCall(Mov9ToOP1)
	bCall(InvOP1S)
	ld hl, &FP_2A
	bCall(Mov9ToOP2)
	bCall(FPDiv)
	ld hl, &OP1
	ld de, &FP_AOS
	bCall(Mov9B)
	
	ld hl, &FP_Discriminant
	bCall(Mov9ToOP1)

	; Essentially find the ABS of OP1
	; by taking the first byte and making
	; it zero
	xor a
	ld (&OP1), a

	bCall(SqRoot)
	ld hl, &FP_2A
	bCall(Mov9ToOP2)
	bCall(FPDiv)

	; OP1 now has sqrt(abs(b^2-4ac))/2a
;;; 	ld hl, &OP1
	ld de, &FP_S1C
;;; 	bCall(Mov9B)
	bCall(MovFrOP1)
	
;;; 	ld hl, &OP1
	ld de, &FP_S2C
;;; 	bCall(Mov9B)
	bCall(MovFrOP1)

	;=--------------=
	
	ld hl, &FP_AOS
	ld de, &FP_S1R
	bCall(Mov9B)
	ld hl, &FP_AOS
	ld de, &FP_S2R
	bCall(Mov9B)

	; The Positive Complex Portion
	; in FP it's: 00000110
	ld hl, &FP_S1C
	ld a, @0x0C
	ld (hl), a

	; The Negative Complex Portion
	; in FP it's: 10000110
	; (The first bit; bit 7; is sign)
	ld hl, &FP_S2C
	ld a, @0x8C
	ld (hl), a

	; Make them complex
	ld hl, &FP_S1R
	ld a, (hl)
	or @0x0C
	ld (hl), a
	
	ld hl, &FP_S2R
	ld a, (hl)
	or @0x0C
	ld (hl), a

	ld hl, &FP_S1R
	bCall(Mov9ToOP1)
	ld hl, &FP_S1C
	bCall(Mov9ToOP2)
	bCall(StoX)

	; 
	ld hl, &FP_S2R
	bCall(Mov9ToOP1)
	ld hl, &FP_S2C
	bCall(Mov9ToOP2)
	bCall(StoY)

	jp &show_solutions
	
	ret

	
title:
;;    123456789012345-123456789012345-123456789012345-123456789012345-1234567890123456
;;.str "Quad Form Solver"
.str "   Quadratic     Formula Solver ================by mr pellegrino================"

.fp FP_A
.fp FP_B
.fp FP_C
	
.fp FP_Discriminant
.fp FP_AOS
.fp FP_2A
.fp FP_4AC
	
;; These are twice as big as FP's because they could be complex
FP_Solution1:
.fp FP_S1R
.fp FP_S1C
FP_Solution2:
.fp FP_S2R
.fp FP_S2C
