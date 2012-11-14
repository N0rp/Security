.data

.text
	.globl main
	.type main, @function

main:
	movzbl (%eax), %eax
	cmp		%al, %bl
	call 	0x5

