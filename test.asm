.name prog4
top:
	bCall(RunIndicOff)

	ld hl, &prompt
	bCall(PutS)
	
	call(user_input)

	ld hl, &FP_user_input
	ld de, &FP_myfloat
	ld b, @0x09
	ld c, @0x00
	ldir

	ld hl, &FP_myfloat
	ld de, &OP1
	ld b, @0x09
	ld c, @0x00
	ldir

	bCall(LnX)
;	ld a, @0x46
;	ld (&variabletoken), a
;	call(store_op1)

	bCall(StoX)
	bCall(NewLine)
	ret
	call(store_op1)

.fp FP_myfloat
prompt:
.str "> "
