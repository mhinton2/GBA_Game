@ score.s

/* adds 5 to the score every call */
.global	score
score:
	add r0, r0, #5
	mov pc, lr