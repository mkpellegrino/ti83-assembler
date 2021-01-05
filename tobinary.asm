	.name tobinary
	ld hl, &prompt
	bCall(PutS)
	call &dont_check_for_decimal_points
	call &dont_check_for_negatives
	
	call &user_input
	ld hl, &FP_user_input
        bCall(Mov9ToOP1)
	
	call &convop1b
	
	push de
	call &toBinary

	ld hl, &header
	bCall(PutS)
	ld hl, &binary_string
	bCall(PutS)
	ld hl, &footer
	bCall(PutS)
	ret



	
toBinary:
	pop de
	pop hl
	push de
	;; 16 ticks
	ld (&value), hl
	ld hl, &binary_string
	
	ld a, (&value_l)

	ld b, @0x02
toBinaryLoop0:	
	push bc
;;; ======================
	ld b, @0x08
toBinaryLoop1:
	sla a
	jr nc, %no_one
	ld (hl), @0x31
no_one:
	inc hl
	djnz %toBinaryLoop1
;;; ======================
	pop bc
	ld a, (&value_h)
	djnz %toBinaryLoop0
	
	
	ret

value:
value_h:
.db 0x00
value_l:	
.db 0x00
binary_string:
binary_string_h:
.chars "00000000"
binary_string_l:
.chars "00000000"
.db 0x00
header:
.str "vvvv    vvvv    "
footer:
.str "    ^^^^    ^^^^"
prompt:
.str "Enter a decimal:"
