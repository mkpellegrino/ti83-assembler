.name tglmode
	bit 2, (iy+@0x00)
	jr z, %toggle1	
	res 2, (iy+@0x00)
	ret
toggle1:
	set 2, (iy+@0x00)
	ret
