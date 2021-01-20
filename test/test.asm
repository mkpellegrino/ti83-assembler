	; a test program compiled with tias v0.9b
	; by: michael k. pellegrino
	;
	; set the name internal to the TI8X
.name testmem
	; get user input and store it in FP_user_input

	; display text1 (the prompt > shown below)
	ld hl, &text1
	bCall(PutS)

	; builtin numeric user input
	call &user_input
	bCall(NewLine)

	; copy the user entered floating point to OP1
	ld hl, &FP_user_input
	bCall(Mov9ToOP1)

	; Store it in the Calculator variable "F"
	ld a, @0x46
	ld (&variabletoken), a
	call &store_op1


	; text saying "User Input -> variable F"
	ld hl, &text2
	bCall(PutS)
	bCall(NewLine)

	
	ld hl, &text3
	bCall(PutS)

	; Put it into User Input FP again
	ld hl, &FP_user_input
	ld de, &OP1
	bCall(Mov9B)

	; Show OP1 on the screen
	call &disp_op1
	bCall(NewLine)


	
	ret
text1:
.str "> "
.db 0
text2:
.str "UI -> F"
.db 0
text3:
.str "OP1="
.db 0
