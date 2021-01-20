.name tsr

	;;  the size of the interrupt routine in bytes
	;; install routine alone (with TI header) is 110 bytes
	;; install routine without header is 32 bytes	
.label routine_size 0x0044
	
	;; this is wehere in memory the program
	;; will live after the installer terminates
.label routine_dest_address 0x9A9A

	;; this is where my variables will be
	;; after the installer terminates
.label mrow 0x9ACB
.label mcol 0x9ACC
.label mstring 0x9ACD
	
	im 1
	di

	ld hl, &my_routine
	ld de, &routine_dest_address
	ld bc, &routine_size
	ldir

	ld hl, #0x9900
	ld a, h
	ld i, a
	ld a, @0x9A
	ld bc, #0x0101
	bCall(MemSet)
	
	im 2
	ei
	ret
	
my_routine:
	ex af, af'
	exx

	;; save row and column
	ld a, (&CurRow)
	ld (&mrow), a
	ld a, (&CurCol)
	ld (&mcol), a

	xor a
	ld (&CurRow), a
	ld (&CurCol), a

	ld hl, &mstring
	bCall(PutS)

	ld a, (&mrow)
	cp @0x00
	jr nz, %skip1
	inc a
skip1:	
	ld (&CurRow), a
	ld a, (&mcol)
	ld (&CurCol), a
	exx
	ex af, af'
	jp #0x0038

old_row:
	.db 0x00
old_col:
	.db 0x00
my_string:
;;;   1234567890ABCDEF-1234567890ABCDE-
.str "PROPERTY OF BHS "
	
