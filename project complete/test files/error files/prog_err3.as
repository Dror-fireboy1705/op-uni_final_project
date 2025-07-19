; a program to check the pre_assembler
MAIN:	prn #16
	mac1
	bne LOOP
	stop
	endmacr
	add #6, r6
	sub #6, r6
	macr 
	add #1, r1
	sub #3, NUM
	endmacr
LOOP:	prn #1
	mac2
; ment to create a note line so that there will be a note line
	cmp r1, r2
	mac1
	macr mov
	add r0, r3
	sub r1, K
	add r2, K
	sub r3, r2
	add r4, r1
	macr mac1
	endmacr
	macr mac2
NUM:	.data 1,2,3,4,5
