;; deck.asm
;;
;; Create a virtual deck of cards
;; shuffle it
;; pop cards off the deck one at a time
;; until the user presses 'y='
;;
;; (C) 2017 - Michael K. Pellegrino
;;
#include    "ti83plus.inc"
#define     progStart   $9D95
.org        progStart-2
.db         $BB,$6D

	bCall( _RunIndicOff )
	;bCall( _RandInit )     ; Seed the Random Number Generator
begin:	; $34 = 52 decimal
	xor a
	ld (top_card),a

	call shuffle
	call sort

	xor a			; CLS
	ld (CurCol),a
	ld (CurRow),a
	bCall(_ClrLCDFull )

	ld hl, text_title
	bCall( _PutS )

	bCall( _NewLine )
	call pressanykey
	bCall( _NewLine )
begin1:
	call dispCard
	call dispkeys
	bCall( _GetKey )
	cp $49
	jr z, end
	cp $44
	jr z, begin
	
	ld a, (top_card)
	cp $34
	jr z, begin
	jr begin1
end:
	bCall( _RunIndicOn )
	ret

dispkeys:
	bCall( _NewLine )
	ld hl, text_keys
	bCall( _PutS )
	bCall( _NewLine )
	ret
	
shuffle:
	ld hl, text_shuffling
	bCall( _PutS )
	bCall( _NewLine )

	ld hl, deck_of_cards
	ld c, $00
	ld b, $34
shuffle0:
	push bc

	push af
	push hl
	
	bCall( _Random )
	ld hl, FP_250
	bCall( _Mov9toOP2 )
	bCall( _FPMult )
	bCall( _Int )
	bCall( _ConvOP1 )
	ld (randbyte), a

	pop hl
	pop af

	ld a, (randbyte)
	ld (hl), a
	inc hl
	inc hl
	inc hl
	inc hl
	pop bc
	djnz shuffle0
	ret
sort:
	ld c,$34	; 52 cards
	dec c		; Note that the first step involves N-1 checks
	ld hl,$0001
sort0:
	ld ix, deck_of_cards
	ld e,h                         ; Bit 0 of E will indicate if there was need to swap
	ld b,c                         ; C holds the number of elements in the current step
sort1:

	ld a,(ix)
	ld d,(ix+4)
	cp d                           ; If A was less than D, the carry will be set
	jr c, sort2

	push hl
	push af

	; save the second card to the buffer
	ld hl, sort_bfr

	ld a, (ix+4)
	ld (hl), a
	inc hl
	ld a, (ix+5)
	ld (hl), a
	inc hl
	ld a, (ix+6)
	ld (hl), a

	; copy first card to second card
	ld a, (ix)
	ld (ix+4), a
	ld a, (ix+1)
	ld (ix+5), a
	ld a, (ix+2)
	ld (ix+6), a
	
	ld hl, sort_bfr
	ld a, (hl)
	ld (ix), a
	inc hl
	ld a, (hl)
	ld (ix+1), a
	inc hl
	ld a, (hl)
	ld (ix+2), a
	pop af
	pop hl
	ld e,l                         ; Swapping is indicated here (L=1)
sort2:
	inc ix
	inc ix
	inc ix
	inc ix
	djnz sort1
	dec e
	jr nz, sort3                   ; If E became zero after DEC, we have to continue
	dec c
	jr nz, sort0
sort3:
	ret


dispCard:
	ld a, (top_card)
	inc a
	ld h, $00
	ld l, a
	bCall( _DispHL )
	ld hl, parentheses_text
	bCall( _PutS )
	
	ld hl, deck_of_cards
	ld a, (top_card)
;; Multiply it by 4
	sla a
	sla a
	ld c, a
	ld b, $00
	add hl, bc		; Move pointer to the (cardNum)th card

	inc hl			; Point hl to the text of card
	bCall( _PutS )
	bCall( _NewLine )
	ld a, (top_card)	; point to next card
	inc a
	ld (top_card), a
	ret

	

;; function - pressanykey
pressanykey:
	ld hl, text_pressanykey
	bCall( _PutS )
	bCall( _GetKey )
	bCall( _NewLine )
	ret

;; DATA SECTION

text_title:
.db "A Deck of Cards ================by mr pellegrino================",0
	
cardNum:
.db $00
	
sort_bfr:
.db $00, $00, $00

top_card:
.db $00
	
text_shuffling:
.db "Shuffling...",0

text_keys:
.db $C1,"Y=] Quit       ",$C1,"Any] Continue  ",$C1,"Graph]Reshuffle",0
	
text_pressanykey:
.db "(press any key)",0

;; Floating Point 250
FP_250:
.db $00, $82, $25, $00, $00, $00, $00, $00, $00, $00, $00

parentheses_text:
.db ") ",0
	
randbyte:
.db $00

deck_of_cards:
.db $00,"AS",$00
.db $00,"2S",$00
.db $00,"3S",$00
.db $00,"4S",$00
.db $00,"5S",$00
.db $00,"6S",$00
.db $00,"7S",$00
.db $00,"8S",$00
.db $00,"9S",$00
.db $00,"TS",$00
.db $00,"JS",$00
.db $00,"QS",$00
.db $00,"KS",$00

.db $00,"AC",$00
.db $00,"2C",$00
.db $00,"3C",$00
.db $00,"4C",$00
.db $00,"5C",$00
.db $00,"6C",$00
.db $00,"7C",$00
.db $00,"8C",$00
.db $00,"9C",$00
.db $00,"TC",$00
.db $00,"JC",$00
.db $00,"QC",$00
.db $00,"KC",$00

.db $00,"AD",$00
.db $00,"2D",$00
.db $00,"3D",$00
.db $00,"4D",$00
.db $00,"5D",$00
.db $00,"6D",$00
.db $00,"7D",$00
.db $00,"8D",$00
.db $00,"9D",$00
.db $00,"TD",$00
.db $00,"JD",$00
.db $00,"QD",$00
.db $00,"KD",$00

.db $00,"AH",$00
.db $00,"2H",$00
.db $00,"3H",$00
.db $00,"4H",$00
.db $00,"5H",$00
.db $00,"6H",$00
.db $00,"7H",$00
.db $00,"8H",$00
.db $00,"9H",$00
.db $00,"TH",$00
.db $00,"JH",$00
.db $00,"QH",$00
.db $00,"KH",$00
