; note
	  	 
	add MAX, r2
	macr m_macr
	prn 48
	endmacr

	sub r2, r8
r1:	rts r0
	cmp *r5, #+3
	m_macr
	add #17, r1
	m_macr
	cmp #-1, #-1, #-1
	rts *register
	m_macr
	add r1, #-3
	jsr r1
