; a program to check the pre_assembler

.entry LOOP
MAIN:	prn #16
	macr mac1
	bne LOOP
	prn STR
	
	stop
	endmacr
	add #6, r6
	sub #6, r6
	macr mac2
	add #1, r1
	rts
	sub #3, NUM
	endmacr
LOOP:	prn #1
	prn #2
	 	 	 
	 	 	 	      		
	prn #3
.extern L3
	mac2
; meant to create a note line so that there will be a note line
	cmp r1, r2
	mac1
	macr mac3
	add r0, L3
	sub r1, K
	add r2, K
	sub r3, r2
	add r4, r1
	dec r5
	inc r6
	dec r7
	endmacr
STR:	.string "abcd"
NUM:	.data 1,2,3,4,5
K:	lea MAIN, r0
	mac3
