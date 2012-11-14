#include <stdlib.h>
#include <stdio.h>


int main(int argc, char ** argv){

	if(argc!=2){
		printf("Please provide one argument.\n");
		exit(0);
	}

	printf("%s\n", argv[1]);
	return 0;
}
