#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

int debug = 0;

char * letter = "!$%&/()=?abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

int USER_LEN = 11;
int SERIAL_LEN = 10;


int main(int argc, char **argv){
	if(argc!=2){
		printf("Please provide exactly one argument\n");
		exit(1);
	}

	if(debug>0)printf("Start\n");

	int usr_len = strlen(argv[1]);
	if(debug>0)printf("Username length: %i\n", usr_len);

	unsigned char username[USER_LEN];	
	//copy username and pad if needed
	int i;
	for(i = 0; i < USER_LEN; i++){
		if(i>=usr_len){
			username[i] = 0x00;
		}else{
			username[i] = argv[1][i];
		}
	}
	if(debug>0)printf("Username is: %s\n", username);
	unsigned char serial[SERIAL_LEN+1];
	serial[SERIAL_LEN] = '\0';

	unsigned long long tmp;
	unsigned long result;	
	unsigned int r0;
	unsigned int r1;
	unsigned char curr;
	unsigned char prev;
	unsigned char new_char;
	serial[0] = username[0];
	for(i = 1; i < SERIAL_LEN; i++){
		curr = username[i];
		prev = serial[i-1];
		if(debug>0)printf(".Current char: %c=%i=0x%x\n", curr, curr, curr);
		if(debug>0)printf("Previous char: %c=%i=0x%x\n", prev, prev, prev);
		result = curr+serial[i-1];
		if(debug>0)printf("c1. %i=0x%x\n", result, result);
		r0 = result;
		tmp = result;
		tmp = (tmp*0xe6c2b449);
		if(debug>0)printf("c2. %f=0x%a\n", tmp, tmp);
		tmp = (tmp/pow(2, 32));//shift>>32
		result = (int)tmp;
		if(debug>0)printf("c3. %u=0x%x (pow-shift)\n", result, result); 
		result = result>>6;
		if(debug>0)printf("c4. %i=0x%x\n", result, result);
		r1 = result;

		result = result<<3;
		if(debug>0)printf("c5. %i=0x%x\n", result, result);
		result += r1;
		if(debug>0)printf("c6. %i=0x%x\n", result, result);
		result = result <<3;
		if(debug>0)printf("c7. %i=0x%x\n", result, result);
		result -= r1;
		if(debug>0)printf("c8. %i=0x%x\n", result, result);
		r0 -= result;
		result = r0;
		if(debug>0)printf("c9. %i=0x%x\n", result, result);
		new_char = letter[result];
		if(debug>0)printf("Serial Char: %c=%i=0x%x\n", new_char, new_char, new_char);
		serial[i] = new_char;
	}


		prev = serial[SERIAL_LEN-2];
		
        if(debug>0)printf(".Current char: %c=%i=0x%x\n", curr, curr, curr);
        if(debug>0)printf("Previous char: %c=%i=0x%x\n", prev, prev, prev);
        result = prev;
		result = ~result;
        if(debug>0)printf("c1. %i=0x%x\n", result, result);
        r0 = result;
        tmp = result;
        tmp = (tmp*0xe6c2b449);
        if(debug>0)printf("c2. %f=0x%a\n", tmp, tmp);
        tmp = (tmp/pow(2, 32));//shift>>32
        result = (int)tmp;
        if(debug>0)printf("c3. %u=0x%x (pow-shift)\n", result, result);
        result = result>>6;
        if(debug>0)printf("c4. %i=0x%x\n", result, result);
        r1 = result;

        result = result<<3;
        if(debug>0)printf("c5. %i=0x%x\n", result, result);
        result += r1;
        if(debug>0)printf("c6. %i=0x%x\n", result, result);
        result = result <<3;
        if(debug>0)printf("c7. %i=0x%x\n", result, result);
        result -= r1;
        if(debug>0)printf("c8. %i=0x%x\n", result, result);
        r0 -= result;
        result = r0;
        if(debug>0)printf("c9. %i=0x%x\n", result, result);
        new_char = letter[result];
        if(debug>0)printf("Serial Char: %c=%i=0x%x\n", new_char, new_char, new_char);
        serial[SERIAL_LEN-1] = new_char;


	if(debug>0)printf("Found result\n");
	if(debug>0)printf("Result has length: %i\n", strlen(serial));
	printf("%s", serial);
	if(debug>0)printf("\n");
	return 0;
}
