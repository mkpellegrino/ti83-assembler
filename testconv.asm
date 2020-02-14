.name testconv
	bCall(RunIndicOff)
	ld hl, &prompt_txt
	bCall(PutS)
	bCall(CursorOn)
	call &user_input
	bCall(CursorOff)
	ld hl, &FP_user_input
	bCall(Mov9ToOP1)
	call &convop1b
	ex de, hl
	bCall(DispHL)
	bCall(NewLine)
	ret

prompt_txt:
.str "> "
