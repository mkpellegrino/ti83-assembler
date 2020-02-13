.name convop1b
;;convop1b_test:
;; Example
;; load the value: 65535
;;	ld h, @0xFF
;;	ld l, @0xFF
;;	bCall(SetXXXXOP2)
;;	ld hl, &OP2
;;	ld de, &FP_convop1_input
;;	bCall(Mov9B)
	
;;	call &convop1b
	
;;	ld hl, (&convop1_output)
;;	bCall(DispHL)
;;	bCall(NewLine)
	
;;	ret

convop1b:
	push af
	push bc
	;push de
	push hl
	
	ld hl, &FP_convop1_input
	bCall(Mov9ToOP1)

	; compare it with 9999 decimal
	ld hl, &convop1b_max_float
	bCall(Mov9ToOP2)
	bCall(CpOP1OP2)

	jr nc, %too_big

	; it must be not too big so, use syscall
	bCall(ConvOP1)
	
	ld h, d
	ld l, e

	ld (&convop1_output), hl
	ld de, hl
	jr %convop1b_end
convop1b_too_big:
	ld hl, &convop1b_max_any
	bCall(Mov9ToOP2)
	bCall(CpOP1OP2)
	jr nc, %convop1b_really_too_big

	ld hl, &convop1b_max_float
	bCall(Mov9ToOP2)
	bCall(FPDiv)
	bCall(Intgr)
	bCall(ConvOP1)

	inc e
	ld b, e
	ld c, @0x00

	ld hl, &FP_convop1_input
	bCall(Mov9ToOP1)
	
	ld hl, #0x0000
	ld de, #0x270F
convop1b_too_big_loop:
	add hl, de
	
	push af
	push bc
	push de
	push hl
	ld hl, &convop1b_max_float
	bCall(Mov9ToOP2)
	bCall(FPSub)
	pop hl
	pop de
	pop bc
	pop af
	djnz %convop1b_too_big_loop

	
	push hl
	bCall(ConvOP1)	
	pop hl
	
	add hl, de

	ld (&convop1_output), hl
	ld de, hl
	jr %convop1b_end
	
convop1b_really_too_big:
	ld hl, #0x0000
	ld (&convop1_output), hl
	ld de, hl
convop1b_end:

	
	pop hl
	;pop de
	pop bc
	pop af
	ret

convop1b_max_float:
.db 0x00
.db 0x83
.db 0x99
.db 0x99
.db 0x00
.db 0x00
.db 0x00
.db 0x00
.db 0x00

convop1b_max_any:
.db 0x00
.db 0x84
.db 0x65
.db 0x53
.db 0x60
.db 0x00
.db 0x00
.db 0x00
.db 0x00
.db 0x00
	
convop1_output:
.dw 0x0000

.fp FP_convop1_input
