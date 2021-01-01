.name tinydeck

 	call &shuffle	
 	call &show_deck
	ret

show_deck:
 	ld b, @0x34
show_deck_top:
	ld hl, &card_to_show
	ld (hl), b
	dec (hl)

	push bc
	ld hl, &deck_of_cards
	ld a, (&card_to_show)
	sla a 
	sla a
	inc a
	ld c, a
	ld b, @0x00
	add hl, bc
	bCall(PutS)
	bCall(NewLine)

	ld hl, &text_pressanykey
	bCall(PutS)
	bCall(GetKey)
	bCall(NewLine)
	pop bc
	
	djnz %show_deck_top
	ret
;;; ========================================
;;; put a random number in front
;;; of the card

shuffle:
;;; 	pusha
	ld hl, &text_shuffling
	bCall(PutS)
	bCall(NewLine)

	ld hl, &deck_of_cards
	ld (&deck_ptr), hl
	
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
;;; 	popa


sort:
 	ld a, @0x33
	ld (&iterations), a
	ld b, a
top0:
	ld hl, &deck_of_cards
	ld (&deck_ptr), hl
	
	push bc

	;; 13 ticks
	ld a, (&iterations)
	;; 4 ticks
	ld b, a
top1:
;;; --------------------------
 	push bc
	
;;; 	ld hl, (&deck_ptr)
	ld a, (hl)
	inc hl
	inc hl
	inc hl
	inc hl
	ld (&deck_ptrp1), hl
	ld d, (hl)
	cp d
 	jr c, %no_swap

swap:
;;; swap deck_ptr and deck_ptrp1
;;; copy 3 bytes from item 1 to tmp1, tmp2, & tmp3
	ld de, &tmp1
	ld hl, (&deck_ptr)
	ldi
	ldi
	ldi

;;; copy 3 bytes from item 2 to item 1
	ld de, (&deck_ptr)
	ld hl, (&deck_ptrp1)
	ldi
	ldi
	ldi
	
;;; copy from tmp1, tmp2, and tmp3 into item 2
	ld de, (&deck_ptrp1)
	ld hl, &tmp1
	ldi
	ldi
	ldi
	
no_swap:
;;; move the deck pointer to next card	
	;; 16 ticks
	ld hl, (&deck_ptr)
	;; 6 ticks
	inc hl
	;; 6 ticks
	inc hl
	;; 6 ticks
	inc hl
	;; 6 ticks
	inc hl
	;; 16 ticks
	ld (&deck_ptr), hl
;;; --------------------------
	pop bc
	djnz %top1

	;; 10 ticks
	ld hl, &iterations
	;; 11 ticks
	dec (hl)

	;; 13 ticks
	;; ld a, (&iterations)
	;; 4 ticks
	;; dec a
	;; 13 ticks
	;; ld (&iterations), a
	
	pop bc
	djnz %top0

	
	ret

randbyte:	
card_to_show:
tmp1:
	.db 0x00
tmp2:
	.db 0x00
tmp3:
	.db 0x00

deck_ptr:
.dw 0x0000
deck_ptrp1:
.dw 0x0000
iterations:
.db 0x00

	

text_pressanykey:
.str "(press a key)"

text_shuffling:
.str "(shuffling)"

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
