#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h> //include O_RDONLY

#define MAX_FILE_SIZE 512

int main(int argc, char **argv){



	char *shellcode_file;

	if(argc<2){
		printf("Please provide the name of the shellcode file.\n");
		exit(0);
	}

	shellcode_file = argv[1];

//load file into buffer and the shellcode is executed
//void execute_shellcode(char *shellcode_file){

	unsigned char file_buffer[MAX_FILE_SIZE];
	int fsize;
	int file;
	file = open(shellcode_file, O_RDONLY);
	if(file){
		fsize = read(file, file_buffer, MAX_FILE_SIZE);
	}else{
		printf("Error opening file.\n");
		exit(0);
	}
	close(file);

	printf("Shellcode byte size is: %i\n", fsize);
	//copied code into buffer, now replacing zeros with nops
	int i;
	int replacements = 0;
	for(i = 0; i<fsize; i++){
		if(file_buffer[i]==0){
			file_buffer[i] = 0x90;
			replacements++;
		}
	}
	printf("Replaced %i zeros with nops\n", replacements);
	
	
	void (* funcptr) (void) = (void (*) (void)) file_buffer;

	printf("Executing shellcode:\n");
	funcptr();

	return 0;
}
