.name looptst2

;;; this would be called from the main program
;;; it stores the address of what is to be looped
;;; in where_to
;;; vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	ld hl, &commands
	push hl
	ld hl, #0x0007
	push hl
	loop

;;;  press enter to continue
	call &user_input

;;; reset i
	ld hl, #0x0000
	ld (&i), hl


	
	ld hl, &commands
	push hl
	ld hl, #0x00FF
	push hl
	loop

	
	ret

commands:
 	ld hl, (&i)
 	push hl
 	bCall(DispHL)
 	bCall(NewLine)
 	pop hl
 	inc hl
 	ld (&i), hl
	ret
 i:
 .dw 0x0000
