#include <stdio.h>#include <stdlib.h>#include <stdint.h>#include <unistd.h>void bitout_ui8(uint8_t a){	int i;	for (i = 0; i < 8; i++){		printf("%d",(a>>(7-i))&0x1);	}}int main(int argc, char** argv){	uint16_t pidfilter = 0;	char pidfilter_on='n';	char wait = 'y';	char* filename = NULL;	int c;		while ((c = getopt(argc,argv,"i:p:c")) != -1){		printf("c: %c\n",c);		switch (c){			case 'p':				pidfilter = atoi(optarg);				pidfilter_on='y';				printf("set pidfilter to: %u\n",pidfilter);				break;			case 'i':				filename = optarg;				printf("set filename to: %s\n",filename);				break;			case 'c':				wait = 'n';				break;			default:				abort();		}	}	if (filename == NULL){		printf("usage: %s [-p n] -i filename\n",argv[0]);		return 1;	}		FILE *f;	char* data = (char*)malloc(188);	int chk = 0;	int i;	printf("opening '%s'...\n",filename);	f = fopen(filename,"r");		int oldsize = ftell(f);	while (!feof(f) && chk != 0x47){		chk = fgetc(f);	}	int newsize = ftell(f);	if (oldsize != newsize) { fseek(f,-1,SEEK_CUR); oldsize--; }	if (feof(f)){		fclose(f);		printf("EOF before hitting mainloop.\n");		return 1;	}		while (!feof(f)){		fread(data,188,1,f);		uint16_t PID = (data[1] & 0x1F) << 8 | (data[2] && 0xff);		if (pidfilter_on == 'n' || pidfilter == PID){			printf("dumping a PID 0x%04x packet.\n",PID);			i=0;			while (i < 188){				printf("0x%04x ",i);				int j = i;				while (j < i+4 && j < 188){					bitout_ui8(data[j]);					printf(" ");					j++;				}				i = j;				printf("\n");			} 			printf("\n\n");			if (wait == 'y') {				if (fgetc(stdin) == 'q') return 0;			}		} else {			printf("skipping PID 0x%04x packet.\n",PID);		}	}		fclose(f);	return 0;}