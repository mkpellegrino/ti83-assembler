.name factors
start:
;;; 	call &fully_clear_screen
	ld hl, &title
	bCall(PutS)
	
	ld hl, &prompt
	bCall(PutS)
	
	bCall(ZeroOP1)
	ld de, &FP_last_integer_factor
	bCall(MovFrOP1)
	
;;; Get a Value from the User (no negatives or decimals!)
	call &dont_check_for_decimal_points
	call &dont_check_for_negatives
	call &user_input
	ld hl, &FP_user_input
	ld de, &FP_x
	bCall(Mov9B)
	bCall(NewLine)

	ld hl, &FP_x
	bCall(Mov9ToOP1)

	bCall(TimesPt5)
	bCall(Plus1)
	bCall(Int)
	ld de, &FP_end_at
	bCall(MovFrOP1)

	bCall(OP2Set2)
 	ld hl, &OP2
	ld de, &FP_d
	bCall(Mov9B)
	bCall(RunIndicOn)

	
topofloop:
 	ld hl, &FP_d
 	bCall(Mov9ToOP2)

	ld hl, &FP_last_integer_factor
	bCall(Mov9ToOP1)
	
	bCall(CpOP1OP2)
	jr z, %end

 	ld hl, &FP_d
 	bCall(Mov9ToOP2)
	ld hl, &FP_x
	bCall(Mov9ToOP1)

	bCall(FPDiv)

	ld de, &FP_q
	bCall(MovFrOP1)

	bCall(Int)

	ld de, &FP_integer
	bCall(MovFrOP1)

	ld hl, &FP_q
	bCall(Mov9ToOP2)

	bCall(FPSub)

	bCall(ZeroOP2)

	bCall(CpOP1OP2)
	call z, &disp_divisor

;; compare divisor and quotient
;; if they're the same, then end
	ld hl, &FP_q
	bCall(Mov9ToOP1)
 	ld hl, &FP_d
 	bCall(Mov9ToOP2)
	bCall(CpOP1OP2)
;;; 	jr z, %end
	ret z
	
;; increase divisor
	bCall(OP1Set2)
 	ld hl, &FP_d
	bCall(Mov9ToOP1)
	bCall(Plus1)

	ld de, &FP_d
	bCall(MovFrOP1)

;; Compare the Divisor with (User Input/2)+1
;; if they're = then end
	ld hl, &FP_end_at
	bCall(Mov9ToOP2)
;; OP1 already contains FP_d
	;ld hl, FP_d
	;bCall( _Mov9ToOP2 )
	bCall(CpOP1OP2)


;;; 	jr z, %end
	ret z
	jr %topofloop
end:	
	ret
	

;; display contents of OP1
disp_divisor:
	pusha
	ld hl, &FP_d
	bCall(Mov9ToOP1)
	ld a, @0x06
	bCall(FormReal)	
	ld hl, &OP3		
	bCall(PutS)		
	ld hl, &text_times
	bCall(PutS)
	ld hl, &FP_q
	bCall(Mov9ToOP1)
	ld a, @0x06
	bCall(FormReal)	
	ld hl, &OP3		
	bCall(PutS)		
	bCall(NewLine)

	ld a, (&BY_line_count)
	cp @0x06
	call z, &pressanykey
	ld hl, &BY_line_count
	inc (hl)
;;; 	ld a, (&BY_line_count)
;;; 	inc a
;;; 	ld (&BY_line_count), a
;; save this as the last integer factor
	ld hl, &FP_q
	ld de, &FP_last_integer_factor
	bCall(Mov9B)
	popa
	ret

;; function - pressanykey
pressanykey:
	pusha
	ld hl, &text_pressanykey
	bCall(PutS)
	bCall(GetKey)
 	bCall(NewLine)
	ld a, @0xFF
	ld (&BY_line_count), a
	popa
	ret


;; DATA SECTION
text_pressanykey:
;;    123456789012345-123456789012345-123456789012345-
.str "(press any key)"

readkeyA_byte:
.db 0x00
;;    123456789012345-123456789012345-123456789012345-
title:
.str " Factor Program by mr pellegrino=--------------="
prompt:
;;    123456789012345-123456789012345-123456789012345-
.str "> "

text_times:
.str " x "
	
;; The user input
.fp FP_x

;; the Quotient 
.fp FP_q

;; the Divisor (Starts as 2)
.fp FP_d
.fp FP_integer
.fp FP_end_at
.fp FP_last_integer_factor

BY_line_count:
.db 0x00
