.data

.text
	.globl main
	.type main, @function

#EDI is used to store start of string
#ESI is used to store the end of the string

main:
jump:
	jmp	before_string		#need to jump to the 'call'

store_string_start:
	#store the begining of the string in EDI
	pop 	%edi 		
	#do not jump here because it creates errors...darn
store_string_end:
	mov	(%esp), %esi
	sub	$6, %esi

print_string:
	sub	%eax, %eax
	sub	%ebx, %ebx
	sub	%edx, %edx
	movb	$4, %al			#sys call 4:write
	movb 	$1, %bl			#out-stream
	mov 	%edi, %ecx
	#add	%edi, %edx		#string size
	movb	$28, %dl
	int 	$0x80			#print string

do_exit:
	subl	%eax, %eax		#clear eax
	subl	%ebx, %ebx		#clear ebx

	ret

before_string:
	call 	store_string_start		#jump back so that the string is in the %esp
	.string "Hello! I am a simple virus!\n"
