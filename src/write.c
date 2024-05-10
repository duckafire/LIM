#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"

void printInFile(FILE *origin, FILE *newFile, char *libName){
	// get library functions, variables and tables
	stage_01_define(origin, newFile, libName);
	
	// remover unnercessary spaces and tabulations
	// stage_02_(origin, newFile);
	
	// replace lua function by reference; add "do" block
	// stage_03_(origin, newFile);
	
	// compact all, expect "defineds" and reservad words 
	// stage_04_(origin, newFile);
}

static void stage_01_define(FILE *origin, FILE *newFile, char *libName){
	char word[9], cc;

	int _fscanf(void){
		memset(word, '\0', 9);
		return (fscanf(origin, "%8[^ \n.]", word));
	}

	while(_fscanf() != -1){
		// declaration of "tool"
		if(strcmp(word, "local") == 0 && strcmp(word, "_G")){
			_fscanf();
			if(strcmp(word, "function") == 0){
				_fscanf();
				fprintf(newFile, "%s.%s=function", libName, word);
			}else{
				fprintf(newFile, "^%s", word);
			}
		}else{
			fprintf(newFile, "%s", word);
		}

		// print the rest of the line
		while((cc = fgetc(origin)) != '\n') fputc(cc, newFile);
		fputc('\n', newFile);
	}
}
