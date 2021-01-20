.name memclear			; just a test of the memclear macro

	ld hl, &title
	bCall(PutS)
	ld b, @0x02
loop0:
	push bc
	ld b, @0x10
	ld hl, &data
loop_top:
	ld a, (hl)
	add a, @0x30
	inc hl
	push hl
	push bc
	bCall(PutC)
	pop bc
	pop hl
	djnz %loop_top

;;; now clear some of that memory
	ld hl, &st
	ld (&function_mem_clear_address), hl
	ld a, @0x05
	ld (&function_mem_clear_number_of_bytesL), a
	call &mem_clear
	pop bc
	djnz %loop0

	
	ret

title:
	.str "mem clear test  "

data:
	.db 0x09
	.db 0x08
	.db 0x07
	.db 0x06
	.db 0x05
	.db 0x04
st:
	.db 0x03
	.db 0x02
	.db 0x01
	.db 0x02
	.db 0x03
ed:
	.db 0x04
	.db 0x05
	.db 0x06
	.db 0x07
	.db 0x08
	
