.name carddeck
	bCall(RunIndicOff)
	;bCall( _RandInit )     ; Seed the Random Number Generator
begin:	
	xor a
	ld (&top_card), a

	
	call &shuffle
	call &sort

	xor a
	ld (&CurCol), a
	ld (&CurRow), a
	bCall(ClrLCDFull)

	ld hl, &text_title
	bCall(PutS)

	bCall(NewLine)
	call &pressanykey
	bCall(NewLine)
begin1:
	call &dispCard
	call &dispkeys
	bCall(GetKey)
	cp @0x49
	jr z, %end
	cp @0x44
	jr z, %begin
	
	ld a, (&top_card)
	cp @0x34
	jr z, %begin
	jr %begin1
end:
	bCall(RunIndicOn)
	ret

dispkeys:
	bCall(NewLine)
	ld hl, &text_keys
	bCall(PutS)
	bCall(NewLine)
	ret
	
shuffle:
	bCall(RunIndicOn)

	ld hl, &text_shuffling
	bCall(PutS)
	bCall(NewLine)

	ld hl, &deck_of_cards
	ld c, @0x00
	ld b, @0x34
shuffle0:
	push bc

	push af
	push hl
	
	bCall(Random)
	ld hl, &FP_250
	bCall(Mov9ToOP2)
	bCall(FPMult)
	bCall(Int)
	bCall(ConvOP1)
	ld (&randbyte), a

	pop hl
	pop af

	ld a, (&randbyte)
	ld (hl), a
	inc hl
	inc hl
	inc hl
	inc hl
	pop bc
	djnz %shuffle0
	bCall(RunIndicOff)

	ret
sort:
	bCall(RunIndicOn)
	; 52 cards
	ld c, @0x34	
	; Note that the first step involves N-1 checks
	dec c		
	ld hl, #0x0001
sort0:
	ld ix, &deck_of_cards
	; Bit 0 of E will indicate if there was need to swap
	ld e, h
	; C holds the number of elements in the current step
	ld b, c                         
sort1:

	ld a, (ix+@0x00)
	ld d, (ix+@0x04)
                           ; If A was less than D, the carry will be set
	cp d
	jr c, %sort2

	push hl
	push af

	; save the second card to the buffer
	ld hl, &sort_bfr

	ld a, (ix+@0x04)
	ld (hl), a
	inc hl
	ld a, (ix+@0x05)
	ld (hl), a
	inc hl
	ld a, (ix+@0x06)
	ld (hl), a

	; copy first card to second card
	ld a, (ix+@0x00)
	ld (ix+@0x04), a
	ld a, (ix+@0x01)
	ld (ix+@0x05), a
	ld a, (ix+@0x02)
	ld (ix+@0x06), a
	
	ld hl, &sort_bfr
	ld a, (hl)
;;; 	ld (ix), a
	ld (ix+@0x00), a
	inc hl
	ld a, (hl)
	ld (ix+@0x01), a
	inc hl
	ld a, (hl)
	ld (ix+@0x02), a
	pop af
	; Swapping is indicated here (L=1)
	pop hl
	ld e, l                         
sort2:
	inc ix
	inc ix
	inc ix
	inc ix
	djnz %sort1
	dec e
	; If E became zero after DEC, we have to continue
	jr nz, %sort3                   
	dec c
	jr nz, %sort0
sort3:
	bCall(RunIndicOff)
	ret


dispCard:
	ld a, (&top_card)
	inc a
	ld h, @0x00
	ld l, a
	bCall(DispHL)
	ld hl, &parentheses_text
	bCall(PutS)
	
	ld hl, &deck_of_cards
	ld a, (&top_card)
;; Multiply it by 4
	sla a
	sla a
	ld c, a
	ld b, @0x00
		; Move pointer to the (cardNum)th card
	add hl, bc
			; Point hl to the text of card
	inc hl
	bCall(PutS)
	bCall(NewLine)
	ld hl, &top_card
	inc (hl)
	ret

	

;; function - pressanykey
pressanykey:
	ld hl, &text_pressanykey
	bCall(PutS)
	bCall(GetKey)
	bCall(NewLine)
	ret

;; DATA SECTION

text_title:
.str "A Deck of Cards ================by mr pellegrino================"
	
cardNum:
	.db 0x00
	
sort_bfr:
	.db 0x00
	.db 0x00
	.db 0x00

top_card:
.db 0x00
	
text_shuffling:
	.str "Shuffling..."

text_keys:
 	.db 0xC1
	.chars "Y=]    Quit    "
	.db 0xC1
	.str "Graph] Shuffle   "
text_pressanykey:
.str "(press any key)"

	;; Floating Point 250

FP_250:
	.db 0x00
	.db 0x82
	.db 0x25
	.db 0x00
	.db 0x00
	.db 0x00
	.db 0x00
	.db 0x00
	.db 0x00
	.db 0x00
	.db 0x00


parentheses_text:
.str ") "
	
randbyte:
.db 0x00

deck_of_cards:
	.db 0x00
	.str "AS"
	.db 0x00
	.str "2S"
	.db 0x00
	.str "3S"
	.db 0x00
	.str "4S"
	.db 0x00
	.str "5S"
	.db 0x00
	.str "6S"
	.db 0x00
	.str "7S"
	.db 0x00
	.str "8S"
	.db 0x00
	.str "9S"
	.db 0x00
	.str "TS"
	.db 0x00
	.str "JS"
	.db 0x00
	.str "QS"
	.db 0x00
	.str "KS"

	.db 0x00
	.str "AC"
	.db 0x00
	.str "2C"
	.db 0x00
	.str "3C"
	.db 0x00
	.str "4C"
	.db 0x00
	.str "5C"
	.db 0x00
	.str "6C"
	.db 0x00
	.str "7C"
	.db 0x00
	.str "8C"
	.db 0x00
	.str "9C"
	.db 0x00
	.str "TC"
	.db 0x00
	.str "JC"
	.db 0x00
	.str "QC"
	.db 0x00
	.str "KC"

	.db 0x00
	.str "AD"
	.db 0x00
	.str "2D"
	.db 0x00
	.str "3D"
	.db 0x00
	.str "4D"
	.db 0x00
	.str "5D"
	.db 0x00
	.str "6D"
	.db 0x00
	.str "7D"
	.db 0x00
	.str "8D"
	.db 0x00
	.str "9D"
	.db 0x00
	.str "TD"
	.db 0x00
	.str "JD"
	.db 0x00
	.str "QD"
	.db 0x00
	.str "KD"

	.db 0x00
	.str "AH"
	.db 0x00
	.str "2H"
	.db 0x00
	.str "3H"
	.db 0x00
	.str "4H"
	.db 0x00
	.str "5H"
	.db 0x00
	.str "6H"
	.db 0x00
	.str "7H"
	.db 0x00
	.str "8H"
	.db 0x00
	.str "9H"
	.db 0x00
	.str "TH"
	.db 0x00
	.str "JH"
	.db 0x00
	.str "QH"
	.db 0x00
	.str "KH"

