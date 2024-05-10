#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"

void printInFile(FILE *origin, FILE *newFile, char *libName){
	// get library functions, variables and tables; remove unnecessary line feed
	stage_01_define(origin, newFile, libName);
	
	// remover unnercessary spaces and tabulations
	// stage_02_(origin, newFile);
	
	// replace lua function by reference; add "do" block
	// stage_03_(origin, newFile);
	
	// compact all, expect "defineds" and reservad words 
	// stage_04_(origin, newFile);
}

static void stage_01_define(FILE *origin, FILE *newFile, char *libName){
	char init[6], func[9], name[50], cc;
	int qtt = 0;

	memset(init, '\0',  6); // local/_G
	memset(func, '\0',  9); // function
	memset(name, '\0', 50); // function name

	// remove extension of the library name
	int len = strlen(libName) - 8;
	char *lib;

	lib = malloc(len);
	memset(lib, '\0', len);
	
	for(int i = 0; i < len; i++) lib[i] = libName[i];

	// get local/_G
	while(fscanf(origin, "%6[^\n. ]", init) != -1){
		
		if(strcmp(init, "local") == 0 || strcmp(init, "_G") == 0){
			clearSpace(origin);
			// get function reservad word
			fscanf(origin, "%8[^\n]", func);

			if(strcmp(func, "function") == 0){
				clearSpace(origin);
				// get name of the function
				fscanf(origin, "%50[^\n(]", name);

				fprintf(newFile, "%s.%s=function", lib, name);
			}else{
				fseek(origin, -strlen(func), SEEK_CUR);
				fprintf(newFile, "^%s ", init);
			}
		}else{
			fseek(origin, -strlen(init), SEEK_CUR);
		}

		qtt = 0;
		while((cc = fgetc(origin)) != '\n'){
			qtt++;
			fputc(cc, newFile);
		}
		if(qtt > 0) fputc('\n', newFile);

		memset(init, '\0',  6);
		memset(func, '\0',  9);
		memset(name, '\0', 50);
	}

	free(lib);
}
