; file ps.as

MAIN:	add r3, LIST
STR:	.string "abcd"
LOOP:	prn #48
	lea STR, r6
	inc r6
	.data 1,2,3
	sub r1, r4
	cmp r3, #-6
	bne END
K:	.data 31
	add r7, *r6
	clr K
.entry MAIN
	jmp LOOP
END:	stop
LIST:	.data 6, -9
	.data -100
