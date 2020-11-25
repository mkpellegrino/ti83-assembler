.name splitchk
	call &fully_clear_screen
 	bCall(RunIndicOff)

	call &dont_check_for_negatives
 	bCall(RunIndicOff)
	
	ld hl, &title
	bCall(PutS)
;;; 	bCall(NewLine)
	
	ld hl, &how_much
	bCall(PutS)
	call &user_input
;;; 	ld hl, &FP_user_input
	ld de, &FP_how_much
;;; 	bCall(Mov9B)
	bCall(MovFrOP1)

	ld hl, &what_tip
	bCall(PutS)
	call &user_input
;;; 	ld hl, &FP_user_input
	ld de, &FP_what_tip
;;; 	bCall(Mov9B)
	bCall(MovFrOP1)

	call &dont_check_for_decimal_points
	ld hl, &how_many
	bCall(PutS)
	call &user_input
;;; 	ld hl, &FP_user_input
	ld de, &FP_how_many
;;; 	bCall(Mov9B)
	bCall(MovFrOP1)
 	bCall(RunIndicOn)

	ld hl, #0x0064
	bCall(SetXXXXOP2)
	;ld hl, &FP_100
	;bCall(Mov9ToOP2)

	ld hl, &FP_what_tip
	bCall(Mov9ToOP1)

	bCall(FPDiv)
	bCall(Plus1)

	bCall(OP1ToOP2)

	ld hl, &FP_how_much
	bCall(Mov9ToOP1)

	bCall(FPMult)

	ld hl, &FP_how_many
	bCall(Mov9ToOP2)

	bCall(FPDiv)

;;; 	ld hl, &OP1
	ld de, &FP_how_much
;;; 	bCall(Mov9B)
	bCall(MovFrOP1)

	
	ld d, @0x02
	bCall(Round)
	ld hl, &out
	bCall(PutS)
	call &disp_op1
	
	ret


.fp FP_how_much

.fp FP_how_many

.fp FP_what_tip


title:
;;;   123456789ABCDEF-123456789ABCDEF-123456789ABCDEF-
.str "split the check by mr pellegrino=--------------="
out:
.str "each pay: "
how_much:
.str "total> "
how_many:
.str "# people> "
what_tip:
.str "% tip> "
