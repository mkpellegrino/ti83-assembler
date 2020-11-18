.name prjct
	
        bCall(RunIndicOff)

;;;  Put the Calculator into Degree Mode
;;; set 2, (iy+0x00)
	call &degree_mode
prjct_top:	
        call &fully_clear_screen
        ld hl, &title
        bCall(PutS)
;;; Get Gravitational Constant
	ld hl, &txt1		
	bCall(PutS)
	call &user_input

;;; Compare OP1 with Zero
;;; if it is equal to then end
	bCall(OP2Set0)
	bCall(CpOP1OP2)
	jr nz, %dont_end_yet
	ld hl, &txt5
	bCall(PutS)
	bCall(NewLine)
	bCall(RunIndicOn)
	ret
dont_end_yet:	
;;; Make sure its negative
	bCall(FPSquare)
	bCall(SqRoot)
	bCall(InvOP1S)
	
        ld de, &FP_g
	bCall(MovFrOP1)

;;; Calculate A (1/2)*G
	bCall(TimesPt5)
	ld de, &FP_A
	bCall(MovFrOP1)

	
;;; Get Theta
	ld hl, &txt4
	bCall(PutS)
	
	call &check_for_negatives
	call &user_input
;;; convert the degree measure to radian measure before storing it
;;; bCall(DToR)
        ld de, &FP_theta
	bCall(MovFrOP1)
;;; 	bCall(Sin)
	
;;; Get V naught
	ld hl, &txt2
	bCall(PutS)
	call &dont_check_for_negatives
	call &user_input	
        ld de, &FP_vo
        bCall(MovFrOP1)
	
	call &check_for_negatives
;;; Calculate Vv (B)
	ld hl, &FP_theta
	bCall(Mov9ToOP1)
	bCall(Sin)
	ld hl, &FP_vo
	bCall(Mov9ToOP2)
	bCall(FPMult)
	
	ld de, &FP_Vv
	bCall(MovFrOP1)
;;; Calculate Vh
	ld hl, &FP_theta
	bCall(Mov9ToOP1)
	bCall(Sin)
	
;;; The Cosine gets put into OP2
	ld hl, &FP_vo
	bCall(Mov9ToOP1)

	bCall(FPMult)


	ld de, &FP_Vh
	bCall(MovFrOP1)
	
;;; so far all is correct mathematically	
;;; Get Initial Height (C)

	ld hl, &txt3
	bCall(PutS)
	call &user_input
        ld de, &FP_ho
	bCall(MovFrOP1)

;;; Check to make sure Discriminant
;;; is positive, otherwise error

;;; 4AC -> FP_4AC
	ld hl, &FP_g
	bCall(Mov9ToOP1)
	bCall(Times2)
	ld hl, &FP_C
	bCall(Mov9ToOP2)
	bCall(FPMult)
	ld de, &FP_4AC
	bCall(MovFrOP1)
	ld hl, &FP_B
	bCall(Mov9ToOP1)
	bCall(FPSquare)
	ld hl, &FP_4AC
	bCall(Mov9ToOP2)
	bCall(FPSub)
	ld de, &FP_Discriminant
	bCall(MovFrOP1)
	
;;; this is so that we also check for zero!
	bCall(InvOP1S)
;;; if OP1>0, z=1  otherwise z=0
	bCall(CkOP1Pos)
	
	jp z, &error1

	bCall(InvOP1S)
	bCall(SqRoot)
	ld de, &OP2
	bCall(MovFrOP1)
	ld hl, &FP_B
	bCall(Mov9ToOP1)
	bCall(InvOP1S)
	
	bCall(FPSub)
	ld hl, &FP_g
	bCall(Mov9ToOP2)	
	bCall(FPDiv)

	ld de, &FP_t
	bCall(MovFrOP1)

	ld hl, &FP_theta
	bCall(Mov9ToOP1)
	bCall(Sin)
;;; Cosine(@) gets put into OP2
	ld hl, &FP_vo
	bCall(Mov9ToOP1)
	bCall(FPMult)
	ld hl, &FP_t
	bCall(Mov9ToOP2)
	bCall(FPMult)

	ld de, &FP_d
	bCall(MovFrOP1)
	
;;; Find AOS
	ld hl, &FP_B
	bCall(Mov9ToOP1)
	bCall(InvOP1S)
	ld hl, &FP_g
	bCall(Mov9ToOP2)
	bCall(FPDiv)

	ld de, &FP_AOS
	bCall(MovFrOP1)

	bCall(FPSquare)
	ld hl, &FP_A
	bCall(Mov9ToOP2)
	bCall(FPMult)
	bCall(PushOP1)

	ld hl, &FP_B
	bCall(Mov9ToOP1)
	ld hl, &FP_AOS
	bCall(Mov9ToOP2)
	bCall(FPMult)

	ld hl, &OP1
	bCall(Mov9ToOP2)
	
	bCall(PopOP1)

	bCall(FPAdd)

	ld hl, &FP_C
	bCall(Mov9ToOP2)
	bCall(FPAdd)

	
	ld de, &FP_hmax
	bCall(MovFrOP1)
	
	ld hl, &time_msg
	bCall(PutS)
	ld hl, &FP_t
	bCall(Mov9ToOP1)
	ld d, @0x03
	bCall(Round)
	call &disp_op1

	ld hl, &dist_msg
	bCall(PutS)
	ld hl, &FP_d
	bCall(Mov9ToOP1)
	ld d, @0x03
	bCall(Round)
	call &disp_op1
	

	ld hl, &tmax_msg
	bCall(PutS)
	ld hl, &FP_AOS
	bCall(Mov9ToOP1)
	ld d, @0x03
	bCall(Round)
	call &disp_op1

	ld hl, &hmax_msg
	bCall(PutS)
	ld hl, &FP_hmax
	bCall(Mov9ToOP1)
	ld d, @0x03
	bCall(Round)
	call &disp_op1
	jr %no_error
;;; ret
error1:
	ld hl, &error_msg1
	bCall(PutS)
	bCall(NewLine)
no_error:
	ld hl, &txt6
	bCall(PutS)
	bCall(GetKey)
	jp &prjct_top
	ret
	
time_msg:
	.str "Time: "
dist_msg:
	.str "Dist: "
tmax_msg:
	.str "Tmax: "
hmax_msg:
	.str "Hmax: "
error_msg1:
.str "no solutions"
title:
;;;   123456789ABCDEF-123456789ABCDEF-123456789ABCDEF-123456789ABCDEF-
.str "prj. motion calcby mr pellegrinoenter g=0 to end----------------"
	
wrn1:	
.str "using: "
txt1:
.str "G> "
txt2:
.str "Vo> "
txt3:
.str "Ho> "
txt4:
.db 0x5B
.db 0x3E
.db 0x20
.db 0x00
txt5:
.str "goodbye"
txt6:
;;;   123456789ABCDEF-123456789ABCDEF-123456789ABCDEF-123456789ABCDEF-
.str "<press any key>" 
;;; Gravitational Contant
	.fp FP_g
;;; Initial Velocity
	.fp FP_vo
;;; Initial Altitude
FP_C:
	.fp FP_ho
;;; Angle
	.fp FP_theta

;;; hang time
	.fp FP_t

;;; distance traveled
	.fp FP_d
	.fp FP_hmax
;;; T-Max
	.fp FP_AOS

	.fp FP_A
FP_B:	
;;; Vertical Velocity
	.fp FP_Vv
;;; Horizontal Velocity
	.fp FP_Vh
;;; Part of Discriminant
	.fp FP_4AC
;;; The Discriminant
	.fp FP_Discriminant

