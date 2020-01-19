;; mortar
#include    "ti83plus.inc"
#define     progStart   $9D95
.org        progStart-2
.db         $BB,$6D

begin:
	;bCall( _RandInit )
	bCall( _Random )
	bCall( _Random )
	bCall( _Random )

	
	call initgraphwindow	; Save current window settings & set our own
	
	set trigDeg,(IY+trigFlags) ; set calc to degrees mode

	bCall( _RunIndicOff )  	; shut off that stupid indicator

	bCall( _ZeroOP1 )
	ld hl, OP1
	ld de, FP_score
	call Store9Bytes

	call cls
	ld hl, title 		; Display the Title Screen
	bCall( _PutS )		; 
	call pressanykey
skiptitle:

        call cls		;
	;jp enterangle		; skip the intro           DEBUG ONLY
	;jp nextgame		; skip part of the intro   DEBUG ONLY
	ld hl, instructions1
	bCall( _PutS )
	call pressanykey
	
	ld hl, instructions2
	bCall( _PutS )
	call pressanykey

	ld hl, instructions3
	bCall( _PutS )
	call pressanykey

nextgame:
	bCall( _Random )	;put a random float into OP1
	ld hl, FP_200		; Between 50 and 250
	bCall( _Mov9toOP2 )	; put 200 into OP2
	bCall( _FPMult )	; mult OP1 * OP2
	ld hl, FP_50		; 
	bCall( _Mov9toOP2 )	;
	bCall( _FPAdd )		; add 50 to result
	ld de, $03
	bCall( _Round )
	ld hl, OP1
	ld de, FP_enemy_loc
	call Store9Bytes


	; generate a random height
	bCall( _Random )	; Rand -> OP1
	ld hl, OP1
	ld de, FP_C		; Temporarily Store this Random # in FP_C
	call Store9Bytes

	ld a, $81		;
	ld (power), a		;
	ld hl, FP_any10		; 10 -> FP_any10
	bCall( _Mov9toOP2 )	; FP_any10 -> OP2
	ld hl, FP_any10		; 
	bCall( _Mov9toOP1 )	; FP_any10 -> OP1
	bCall( _FPAdd )		; 20 -> OP1
	ld hl, FP_C		; Store it Temporarily in FP_C
	bCall( _Mov9toOP2 )
	bCall( _FPMult )	; Multiply it
	ld hl, FP_any10
	bCall( _Mov9toOP2 )
	bCall( _FPAdd )		; Then add 10
	bCall( _FPAdd )		; Then add 10 (again)
	bCall( _Int )
	ld hl, OP1
	ld de, FP_C		; Overwrite the temp value with the permanent one
	call Store9Bytes

	bCall( _OP1Set4 )	; Set the number of mortars remaining  to 5
	bCall( _Plus1 )
	
	ld hl, OP1
	ld de, FP_mortars_remaining
	call Store9Bytes

	ld hl, FP_500
	ld de, FP_gunpowder_remaining
	call Store9Bytes
	
showinfo:
	call dispbriefing
enterangle:
	ld hl, msgangle		;prompt
	bCall( _PutS )		;display
	bCall( _NewLine )	;crlf
	call getuserinput	;input

	ld hl, FP_bfr		; load source address
	ld de, FP_angle		; load dest address
	call Store9Bytes	; copy 9 bytes src->dest
	bCall( _NewLine )	; crlf
	
	ld hl, vnaught
	bCall( _PutS )
	bCall( _NewLine )
	call getuserinput

	ld hl, FP_bfr		; load source address
	ld de, FP_force		; load dest address
	call Store9Bytes	; copy 9 bytes src->dest

	ld hl, FP_gunpowder_remaining
	bCall( _Mov9toOP2 )
	
	ld hl, FP_force
	bCall( _Mov9toOP1 )

	ccf
	bCall( _CpOP1OP2 )	; compare # Mortars left with Zero
	
	jr c, enterangle0p01	; continue if we are using an amount
	jr z, enterangle0p01	; of powder that are supllies support
	
;; The amount of gunpowder we've
;; entered is more than we have
;; so ask again
	call cls
	ld hl, text_notenoughgp
	bCall( _PutS )
	bCall( _NewLine )
	call pressanykey
	jp showinfo

enterangle0p01:
	
	
	; now calculate the Vh and Vv
	; Vh
	
	ld hl, FP_angle	
	bCall(_Mov9toOP1)
	bCall(_Sin)
	
	ld hl, FP_force
	bCall(_Mov9toOP2)
	bCall(_FPMult)		; Now OP1 contains F*sin(@)

	;call dispOP1
	
	ld hl, OP1
	ld de, FP_B
	call Store9Bytes	; Store it in FB_B

	; x=(-B-sqrt(B^2 - 4AC))/(2A)
	
	ld hl, FP_A
	bCall( _Mov9toOP1 )
	ld hl, FP_C
	bCall( _Mov9toOP2 )
	
	bCall( _FPMult )	; now OP1 contains AC
	bCall( _Times2 )
	bCall( _Times2 )	; now OP1 contains 4ac
	bCall( _OP1toOP6 )	; now OP5 = 4ac
	ld hl, FP_B
	bCall( _Mov9toOP1 )
	bCall( _FPSquare ) 	; now OP1 contains B^2
	bCall( _OP6toOP2 )
	bCall( _FPSub )		; Now OP1 contains B^2 - 4AC
	bCall( _SqRoot )	; Now OP1 contains sqrt(b^2 - 4ac)
	bCall( _OP1toOP6 )
	ld hl, FP_B		; additive inverse of OP1 
	bCall( _Mov9toOP1 )	;
	bCall( _InvOP1S )
	bCall( _OP6toOP2 )
	bCall( _FPSub )
	bCall( _OP1toOP6 ) 	; now OP6 has -B-sqrt(B^2-4AC)
	ld hl, FP_A
	bCall( _Mov9toOP1 )
	bCall( _Times2 )
	bCall( _OP1toOP2 )
	bCall( _OP6toOP1 )
	bCall( _FPDiv ) 	; now OP1 has (-B+sqrt(B^2-4AC))/(2A)
	ld hl, OP1
	ld de, FP_t
	call Store9Bytes	; Store result in FP_t

	; now find the horiz dist
	; d=rt
	ld hl, FP_angle
	bCall( _Mov9toOP1 )

	bCall( _Sin )		; now the cos(@) is in OP2

	bCall( _OP2toOP1 )
	;ld de, OP1
	;ld hl, OP2
	;call Store9Bytes	; 
	
	ld hl, FP_force
	bCall( _Mov9toOP1 )

	bCall( _FPMult )	; now the Vh is in OP1

	ld hl, OP1	
	ld de, FP_Vh
	call Store9Bytes	; now the Vh is in FP_Vh
	
	ld hl, FP_t
	bCall( _Mov9toOP2 )
	bCall( _FPMult )	; now the distance travelled is in OP1

	ld hl, OP1
	ld de, FP_dist
	call Store9Bytes	; Store it

	ld hl, FP_C
	ld de, FPG_C
	call Store9Bytes

;; Now Divide FP_A by Vh^2
	ld hl, FP_A
	bCall( _Mov9toOP1 )

	ld hl, FP_Vh
	bCall( _Mov9toOP2 )
	bCall( _FPDiv)		; After this, OP2 is intact
	bCall( _FPDiv )

	ld hl, OP1
	ld de, FPG_A
	call Store9Bytes	; Now FPG_A has -16.09/(distance)^2
	
;; Now Divide FP_B by FP_Vh
	ld hl, FP_B
	bCall( _Mov9toOP1 )
	
	ld hl, FP_Vh
	bCall( _Mov9toOP2 )
	bCall( _FPDiv )

	ld hl, OP1
	ld de, FPG_B
	call Store9Bytes	; Now FPG_B has B/distance

	
enterangle3:
	bCall( _RunIndicOff )  	; shut off that stupid indicator
	bCall( _PDspGrph )	; Clear the Graph Screen?
	call drawenemy
	call graphfunction
	call cls
	
	; check for a hit (proximity)
	ld hl, FP_enemy_loc
	bCall( _Mov9toOP1 )	; enemy_loc -> OP1

	; get it to within 5 units
	bCall( _OP2Set5 )
	bCall( _FPDiv )
	ld de, $0000
	bCall( _Round )
	bCall( _OP2Set5 )
	bCall( _FPMult )

	bCall( _OP1toOP6 )	; Hold onto OP1 in OP3 for a Bit

	ld hl, FP_dist
	bCall( _Mov9toOP1 )	; projectile_dist -> OP1

	; get it to within 5 units
	bCall( _OP2Set5 )
	bCall( _FPDiv )
	ld de, $0000
	bCall( _Round )
	bCall( _OP2Set5 )
	bCall( _FPMult )

	bCall( _OP6toOP2 )
	
	bCall( _CpOP1OP2 )	; Compare OP1 with OP2
;;; $$$$
	jr z, enterangle_direct_hit
	jr nc, enterangle_proud
	
enterangle_shy:
	ld hl, text_shy
enterangle_shy0:
	bCall( _PutS )
	
	ld hl, FP_gunpowder_remaining
	bCall( _Mov9toOP1 )
	ld hl, FP_force
	bCall( _Mov9toOP2 )
	bCall( _FPSub )
	ld hl, OP1
	ld de, FP_gunpowder_remaining
	call Store9Bytes

	bCall( _ZeroOP2 )
	bCall( _CpOP1OP2 )
	jr z, endgame_out_of_gp

	ld hl, FP_mortars_remaining
	bCall( _Mov9toOP1 )
	bCall( _Minus1 )
	ld hl, OP1
	ld de, FP_mortars_remaining
	call Store9Bytes

	bCall( _ZeroOP2 )	;
	bCall( _CpOP1OP2 )	; compare # Mortars left with Zero

	jr z, endgame_out_of_mortars
	call pressanykey
	bCall( _Disp )
	jp showinfo
enterangle_proud:
	ld hl, text_proud
	jp enterangle_shy0
enterangle_direct_hit:

;; Add amount of gunpowder
;; remaining to the FP_score

	ld hl, FP_gunpowder_remaining
	bCall( _Mov9toOP2 )
	ld hl, FP_score
	bCall( _Mov9toOP1 )
	bCall( _FPAdd )
	ld hl, OP1
	ld de, FP_score
	call Store9Bytes


	ld hl, FP_gunpowder_remaining
	bCall( _Mov9toOP1 )
	ld hl, FP_force
	bCall( _Mov9toOP2 )
	bCall( _FPSub )
	ld hl, OP1
	ld de, FP_gunpowder_remaining
	call Store9Bytes

	
	ld hl, text_hit
	bCall( _PutS )
	call pressanykey

	jp endgame
	
endgame_out_of_gp:
	call cls
	ld hl, text_outofgunpowder
	bCall( _PutS )
	bCall( _NewLine )
	call pressanykey
	call cls
	
	call dispbriefing
	jp endgame
	
endgame_out_of_mortars:

	call cls
	ld hl, text_outofmortars
	bCall( _PutS )
	bCall( _NewLine )
	call pressanykey
	call cls
	
endgame:
	call dispbriefing
	call pressanykey
	call cls
	ld hl, text_again
	bCall( _PutS )
	call readkey
	ld a, (readkey_byte)
	cp skYEqu
	jp nz, nextgame
	call restoregraphwindow
	ret

drawenemy:
	push hl
	push de
	push af
	
	ld hl, FP_enemy_loc
	bCall( _Mov9toOp1 )
	bCall( _Minus1 )
	bCall( _Minus1 )
	bCall( _Minus1 )
	
	ld hl, FP_255
	bCall( _Mov9toOp2 )
	bCall( _FPDiv )
	ld hl, FP_93
	bCall( _Mov9toOP2 )
	bCall( _FPMult )
	bCall( _Int )
	bCall( _ConvOP1 )
	ld a, e
	ld (drawenemy_x), a
	; ld a, (drawenemy_x) ;; not needed
	ld (plotXY_x), a
	ld a, $01
	ld (plotXY_y), a
	call plotXY
	
	ld a, (drawenemy_x)
	ld (plotXY_x), a
	ld a, $02
	ld (plotXY_y), a
	call plotXY

	ld a, (drawenemy_x)
	ld (plotXY_x), a
	ld a, $03
	ld (plotXY_y), a
	call plotXY

	ld a, (drawenemy_x)
	ld (plotXY_x), a
	ld a, $04
	ld (plotXY_y), a
	call plotXY
	
	pop af
	pop de
	pop hl
	ret
	
dispOP1:			
	ld a, $06
	bCall( _FormReal )	
	ld hl, OP3		
	bCall( _PutS )		
	bCall( _NewLine )	
	ret

initgraphwindow:	
	ld hl, Xmin		; Save Current State of Window
	ld de, FP_Old_XMin
	call Store9Bytes

	ld hl, Ymin
	ld de, FP_Old_YMin
	call Store9Bytes

	ld hl, Xmax
	ld de, FP_Old_XMax
	call Store9Bytes

	ld hl, Ymax
	ld de, FP_Old_YMax
	call Store9Bytes

	bCall( _ZeroOP1 )	; SET RANDOM Location FOR ENEMY
	ld a, XMINt
	bCall( _StoSysTok )

	bCall( _ZeroOP1 )	; SET RANDOM Location FOR ENEMY
	ld a, YMINt
	bCall( _StoSysTok )

	ld hl, FP_255
	bCall( _Mov9toOP1 )
	ld a, XMAXt
	bCall( _StoSysTok )

	ld hl, FP_255
	bCall( _Mov9toOP1 )
	ld a, YMAXt
	bCall( _StoSysTok )

	set graphDraw, (IY + graphFlags) ; Set Some Graphing Flags
	;set smartGraph_inv, (IY + smartFlags)
	ret
	
restoregraphwindow:
	ld hl, FP_Old_XMin		; Load the window settings for the game
	bCall( _Mov9toOP1 )
	ld a, XMINt
	bCall( _StoSysTok )

	ld hl, FP_Old_YMin
	bCall( _Mov9toOP1 )
	ld a, YMINt
	bCall( _StoSysTok )

	ld hl, FP_Old_XMax
	bCall( _Mov9toOP1 )
	ld a, XMAXt
	bCall( _StoSysTok )

	ld hl, FP_Old_YMax
	bCall( _Mov9toOP1 )
	ld a, YMAXt
	bCall( _StoSysTok )
	ret	
	
graphfunction:			; I know it makes it to here
	ld a, tA
	ld hl, FPG_A
	call storeVariable

	ld a, tB
	ld hl, FPG_B
	call storeVariable

	ld a, tC
	ld hl, FPG_C
	call storeVariable

	; Now create a String of Tokens Ax^2+Bx+C
	ld hl, StringName
	bCall( _Mov9toOP1 )
	bCall( _FindSym )
	jr c, storeEquation
	bCall( _DelVar )
storeEquation:
	ld hl, $08
	bCall( _CreateStrng )
	inc de
	inc de
	ld hl, equation
	ld bc, $08
	ldir

	res graphDraw, (IY + graphFlags)

;; put something at the enemy location
;; on the graph
	
	
	ld hl, StringName
        bCall( _Mov9toOP1 )
        bCall( _PushRealO1 )
        bCall( _DrawCmd )
        bCall( _PopRealO1 )

	set graphDraw, (IY + graphFlags)

	;bCall( _Disp )
	ret

dispbriefing:
	push hl
	push af
	push de
	call cls

	ld hl, briefing_text1
	bCall( _PutS )
	ld hl, FP_score
	bCall( _Mov9toOP1 )
	call dispOP1
	ld hl, briefing_text2
	bCall( _PutS )
	ld hl, FP_mortars_remaining
	bCall( _Mov9toOP1 )
	call dispOP1
	
	ld hl, briefing_text3
	bCall( _PutS )
	ld hl, FP_gunpowder_remaining
	bCall( _Mov9toOP1 )
	call dispOP1

	ld hl, briefing_text4
	bCall( _PutS )
	ld hl, FP_enemy_loc
	bCall( _Mov9toOP1 )
	call dispOP1

	ld hl, briefing_text5
	bCall( _PutS )
	ld hl, FP_C
	bCall( _Mov9toOP1 )
	call dispOP1

	pop de
	pop af
	pop hl

	ret
dispintel:
	push hl
	push af
	push de
	call cls
	ld hl, intel1
	bCall( _PutS )
	
	ld hl, FP_enemy_loc	; Display the Enemy Location
	bCall( _Mov9toOP1 )
	ld a, $07
	bCall( _FormReal )
	ld hl, OP3
	bCall( _PutS )
	bCall( _NewLine )
	ld hl, intel2
	bCall( _PutS )
	call pressanykey
	
	call cls
	ld hl, intel3
	bCall( _PutS )
	ld hl, FP_C		; Display the Altitude
	bCall( _Mov9toOP1 )
	ld a, $07
	bCall( _FormReal )
	ld hl, OP3
	bCall( _PutS )
	bCall( _NewLine )
	ld hl, intel4
	bCall( _PutS )
	call pressanykey

	call cls
;; Display Score
	ld hl, text_score
	bCall( _PutS )
	bCall( _NewLine )
	ld hl, FP_score
	bCall( _Mov9toOP1 )
	call dispOP1
	
	;ld a, $07
	;bCall( _FormReal )
	;ld hl, OP3
	;bCall( _PutS )
	;bCall( _NewLine )

	call pressanykey
	
;; Display Gunpowder Remaining
	call cls
	ld hl, gunpowderremaining1
	bCall( _PutS )
	bCall( _NewLine )


	ld hl, FP_gunpowder_remaining
	bCall( _Mov9toOP1 )
	call dispOP1
	
	ld hl, gunpowderremaining2
	bCall( _PutS )
	bCall( _NewLine )
	call pressanykey

;; Display Mortars Remaining
	call cls
	ld hl, mortarsremaining1
	bCall( _PutS )
	bCall( _NewLine )


	ld hl, FP_mortars_remaining
	bCall( _Mov9toOP1 )
	call dispOP1
	
	ld hl, mortarsremaining2
	bCall( _PutS )
	bCall( _NewLine )
	call pressanykey
	
	call cls
	pop de
	pop af
	pop hl
	ret



	
getuserinput:
	push hl
	push af
	push bc
	push de

	xor a
	ld (text_buffer_length), a
	
	ld hl, text_buffer
	ld (text_buffer_ptr), hl
	
	ld hl, prompt_text
	bCall( _PutS )
readmore:
	call readkeyA
	ld a, (text_buffer_length)
	cp $18
	jp z, buffer_filled
	
	ld a, (readkeyA_byte)
	or a
	jp z, buffer_filled
	jp readmore 

buffer_filled:
	ld a, (text_buffer_length) ; take the last byte off of the equation
	dec a
	ld (text_buffer_length), a
	call create_equation

	ld hl, equationName
	ld de, OP1
	ld bc, $04
	ldir
	bCall( _ParseInp )
	ld hl, OP1
	ld de, FP_bfr
	ld bc, $09
	ldir

	; Delete the equation from memory
	ld hl, equationName	; look for the variable in the vat
	bCall( _Mov9toOP1 )	; using its name (FP_VA)
	bCall( _FindSym )	; 
	bCall( _DelVar )	; and delete it from the VAT

	pop de
	pop bc
	pop af
	pop hl
	ret

readkeyA:
	push af
	push hl
readkeyA0:
	bCall(_GetCSC)		; read the keyboard
	or a			; cp a, $00
	jp z, readkeyA0
	cp sk0
	jp z, readkeyA_zero
	cp sk9
	jp z, readkeyA_nine
	cp sk8
	jp z, readkeyA_eight
	cp sk7
	jp z, readkeyA_seven
	cp sk6
	jp z, readkeyA_six
	cp sk5
	jp z, readkeyA_five
	cp sk4
	jp z, readkeyA_four
	cp sk3
	jp z, readkeyA_three
	cp sk2
	jp z, readkeyA_two
	cp sk1
	jp z, readkeyA_one
	cp skEnter
	jp z, readkeyA_cr
	cp skDecPnt
	jp z, readkeyA_decpt
	;cp skSub
	;jp z, readkeyA_negative
	;cp skChs
	;jp z, readkeyA_negative
	cp skLeft
	jp z, readkeyA_backspace
	cp skDel
	jp z, readkeyA_backspace
	jp readkeyA0

readkeyA1:
	bCall( _PutC )
readkeyA2:
	ld (readkeyA_byte), a
	ld hl, (text_buffer_ptr)
	ld (hl), a
	inc hl
	ld (text_buffer_ptr), hl

	ld a, (text_buffer_length)
	inc a
	ld (text_buffer_length), a
	
	pop hl
	pop af
	ret
readkeyA_zero:
	ld a, $30
	jp readkeyA1
readkeyA_nine:
	ld a, $39
	jp readkeyA1
readkeyA_eight:
	ld a, $38
	jp readkeyA1
readkeyA_seven:
	ld a, $37
	jp readkeyA1
readkeyA_six:
	ld a, $36
	jp readkeyA1
readkeyA_five:
	ld a, $35
	jp readkeyA1
readkeyA_four:
	ld a, $34
	jp readkeyA1
readkeyA_three:
	ld a, $33
	jp readkeyA1
readkeyA_two:
	ld a, $32
	jp readkeyA1
readkeyA_one:
	ld a, $31
	jp readkeyA1
readkeyA_cr:
;; check to see if this is the first byte-- if it is
;; then do nothing
	ld a, (text_buffer_length) ; if text length = 0 just return
	or a
	jp z, readkeyA0
	
	ld a, $00
	jp readkeyA2
readkeyA_decpt:
	ld a, '.'
	bCall( _PutC )
	ld a, tDecPt
	jp readkeyA2
readkeyA_negative:
	ld a, '-'
	bCall( _PutC )
	ld a, tChs
	jp readkeyA2
readkeyA_backspace:
	ld a, (text_buffer_length) ; if text length = 0 just return
	or a
	jp z, readkeyA0
	
;; ;; ; otherwise
	dec a
	ld (text_buffer_length), a

	push af
	ld a, (CurCol)
	dec a
	ld (CurCol),a
	ld a, ' '
	bCall(_PutC)
	ld a, (CurCol)
	dec a
	ld (CurCol),a
	pop af

	push hl
	ld hl, (text_buffer_ptr)
	dec hl
	ld (text_buffer_ptr), hl
	pop hl
	jp readkeyA0
	
create_equation:
	ld hl, equationName	; look for the variable in the vat
	bCall( _Mov9toOP1 )	; using its name (FP_VA)
	bCall( _FindSym )	; 
	jr c, storeEqu		; if it isn't found then create it	
	bCall( _DelVar )	; and delete it from the VAT
storeEqu:
	ld a, (text_buffer_length)
	ld h, $00
	ld l, a			; Bytes of Memory needed to store it
	bCall( _CreateEqu ) 	; de is returned
	inc de
	inc de
	ld hl, text_buffer	; Point hl to start of Text Buffer
	ld a, (text_buffer_length)
	ld b, $00
	ld c, a			; Bytes of Memory needed to store it
	ldir			; copy it to the VAT
	ret
	

;;; FUNCTION cls
cls:
	push af
	push hl
	push bc
	ld b, $0A

clsA:
	bCall(_NewLine)
	djnz clsA
	xor a
	
	ld (CurCol),a
	ld (CurRow),a
	bCall(_ClrLCDFull )

	pop bc
	pop hl
	pop af
	ret

;;; FUNCTION Store9Bytes
Store9Bytes:			; DE = Dest Addr, HL = Src Addr
	ld (store9_hl), hl
	ex de, hl
	ld (store9_de), hl
	push hl
	push de
	push bc
	ld hl, (store9_hl)
	ld de, (store9_de)
	ld bc, $09
	ldir
	pop bc
	pop hl			; Switched the order here because I did an
	pop de			; exchange at the beginning of the func.
	ret
		
plotXY:
	push af
	push bc
	push de
	set fullScrnDraw, (IY + apiFlg4)
	res plotLoc, (IY + plotFlags)
	ld a, (plotXY_x)
	ld b, a
	ld a, (plotXY_y)
	ld c, a
	ld d, $01
	bCall( _IPoint )
	pop de
	pop bc
	pop af
	ret

storeVariable:
	ld (variabletoken), a
	ld de, variabledata
	ld bc, $09
	ldir
	
	push hl
	push de
	push bc
	ld hl, variablename	; look for the variable in the vat
	bCall( _Mov9toOP1 )	; using its name (FP_VA)
	bCall( _FindSym )	; 
	jr c, storeVAR		; if it isn't found then create it
	bCall( _DelVar )	; and delete it from the VAT
storeVAR:
	ld hl, $09		; Bytes of Memory needed to store it
	bCall( _CreateReal ) 	; de is returned	
	ld hl, variabledata	; Point hl to start of Variable 
	ld bc, $09		; Bytes of Memory needed to store it
	ldir			; copy it to the VAT

	; clear out token
	xor a
	ld (variabletoken), a

	pop bc
	pop de
	pop hl
	ret

readkey:
	push af
	push hl
readkey0:
	bCall(_GetCSC)		; read the keyboard
	or a			; cp a, $00
	jr z, readkey0
	ld (readkey_byte), a
	pop hl
	pop af
	ret

pressanykey:
	push hl
	push af
	push de
	push bc
	bCall( _NewLine )
	bCall( _NewLine )
	ld hl, text_pressanykey
	bCall( _PutS )
	bCall( _GetKey )
	bCall( _NewLine )
	bCall( _NewLine )
	pop bc
	pop de
	pop af
	pop hl
	ret


; DATA SECTION
variablename:
.db RealObj
variabletoken:
.db $00, $00, $00
variabledata:
.db $00, $00, $00, $00, $00, $00, $00, $00, $00

StringName:
.db StrngObj,tVarStrng,tStr1,$00,$00
	
equation:
.db tA, tX, tSqr, tAdd, tB, tX, tAdd, tC, $00

;; 10^any power
FP_any10:
.db $00
power:
.db $00,$10,$00,$00,$00,$00,$00,$00,$00,$00,$00

FP_Old_XMin:
.db $80, $81, $12, $00, $00, $00, $00, $00, $00, $00, $00
FP_Old_YMin:
.db $80, $81, $12, $00, $00, $00, $00, $00, $00, $00, $00
FP_Old_XMax:
.db $00, $81, $12, $00, $00, $00, $00, $00, $00, $00, $00
FP_Old_YMax:
.db $00, $81, $12, $00, $00, $00, $00, $00, $00, $00, $00
	
FP_500:
.db $00, $82, $50, $00, $00, $00, $00, $00, $00, $00, $00

FP_93:
.db $00, $81, $93, $00, $00, $00, $00, $00, $00, $00, $00

FP_50:
.db $00, $81, $50, $00, $00, $00, $00, $00, $00, $00, $00

FP_200:
.db $00, $82, $20, $00, $00, $00, $00, $00, $00, $00, $00

FP_255:
.db $00, $82, $25, $50, $00, $00, $00, $00, $00, $00, $00
	
FP_enemy_loc:
.db $00, $81, $00, $00, $00, $00, $00, $00, $00, $00, $00
	
FP_A:
.db $80, $81, $16, $09, $00, $00, $00, $00, $00, $00, $00

	; This should be the Vertical Velocity (Sin(@)*FPS)
FP_B:
.db $00, $81, $00, $00, $00, $00, $00, $00, $00, $00, $00

	; This will be zero (initial height)
FP_C:
.db $00, $81, $00, $00, $00, $00, $00, $00, $00, $00, $00

	; user entered
FP_angle:
.db $00, $81, $00, $00, $00, $00, $00, $00, $00, $00, $00

	; user entered
FP_force:
.db $00, $81, $00, $00, $00, $00, $00, $00, $00, $00, $00
	
	; This will be the horizontal velocity
FP_Vh:
.db $00, $81, $00, $00, $00, $00, $00, $00, $00, $00, $00

	; This will be the time (in seconds)
FP_t:
.db $00, $81, $00, $00, $00, $00, $00, $00, $00, $00, $00

	; This will be the distance travelled horizontally in ft
FP_dist:
.db $00, $81, $00, $00, $00, $00, $00, $00, $00, $00, $00

briefing_text1:
.db "Score:",0
briefing_text2:
.db "Mrtrs Left:",0
briefing_text3:
.db "Pwdr Left:",0
briefing_text4:
.db "Enmy Loc:",0
briefing_text5:
.db "Height:",0
	

msgangle:
;;   123456789012345|
.db "Enter angle in  "
.db "degrees.",0
vnaught:
;;   123456789012345|
.db "Enter amount    "
.db "of gunpowder to "
.db "use.",0
	
title:
;;   1234567890123456
.db "     Cannon     "
.db "   created by   "
.db " mr. pellegrino "
.db "================",0

instructions1:
.db "DIRECTIONS:     "
.db "Enter an angle  "
.db "and amt of      "
.db "gunpowder to use"
.db "in your cannon. ",0

instructions2:
.db "                "
.db "Hit the opponent"
.db "in 5 shots or   "
.db "less and you    "
.db "win!            ",0

instructions3:
.db "Your cannon is  "
.db "on a hill       "
.db "looking down at "
.db "your enemy.     "
.db "                ",0

intel1:
.db "The enemy is    ", 0
intel2:
.db "feet away from  "
.db "your location.  ",0
	
intel3:
.db "You are ",0
intel4:
.db "ft higher than  "
.db "The enemy.",0
	
mortarsremaining1:
.db "You have ", 0
mortarsremaining2:
.db "cannon balls    "
.db "remaining.",0
	
gunpowderremaining1:
.db "You have ", 0
gunpowderremaining2:
.db "ounces of powder"
.db "remaining.",0
	
text_proud:
.db "Uh Oh - You     "
.db "Missed!  Use    "
.db "LESS powder or  "
.db "change the angle",0
	
text_shy:
.db "Uh Oh - You     "
.db "Missed!  Use    "
.db "MORE powder or  "
.db "change the angle",0

text_hit:
.db "You Got It!!!!!!"
.db "Good Show!!!!!!!",0

text_score:
.db "Your Score: ", 0

text_again:
.db "Press Y= to quit"
.db "or any other key"
.db "to play again.  ", 0

text_pressanykey:
.db "(press any key)",0

text_notenoughgp:
.db "You don't have  "
.db "that much       "
.db "gunpowder.  Try "
.db "a lesser amount.",0

text_outofgunpowder:
.db "You have run out"
.db "  of gunpowder  "
.db "                "
.db "g a m e  o v e r",0
	
text_outofmortars:
.db "You have run out"
.db " of cannonballs "
.db "                "
.db "g a m e  o v e r",0

FPG_A: 				;Divide this by Hv^2
.db $80,$81,$16,$09,$00,$00,$00,$00,$00,$00,$00
FPG_B:				;Divide this by Hv
.db $80,$81,$16,$09,$00,$00,$00,$00,$00,$00,$00
FPG_C:				; Same as FP_C
.db $80,$81,$16,$09,$00,$00,$00,$00,$00,$00,$00
		
FP_bfr:
.db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
		
store9_hl:
.dw $0000
store9_de:
.dw $0000

FP_gunpowder_remaining:
.db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00

FP_mortars_remaining:
.db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
	
FP_Score:
.db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
			
plotXY_x:
.db $00
plotXY_y:
.db $00
drawenemy_x:
.db $00

readkey_byte:
.db $00
readkeyA_byte:
.db $00


equationName:
.db EquObj, tVarEqu, tY3, $00
	
text_buffer_length:
.db $00				; out of $18
	
text_buffer_ptr:
.dw $0000
	
text_buffer:			; $18 byte buffer
.db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00

prompt_text:
.db "> ",0
	
