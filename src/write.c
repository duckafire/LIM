#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"

void printInFile(FILE *origin, FILE *newFile, char *libName){
	// set library functions
	stage_01_newName(origin, newFile, libName);
	
	// add "do" block and compact lua functions
	// stage_02_noCommt(origin, newFile);
	
	// remove tabulation, line break, comments and unnecessary spaces
	// stage_03_packLib(origin, newFile);
	
	
	// "compact" "customize" words (player -> p)
	// stage_04_compact(origin, newFile);
}

static void stage_01_newName(FILE *origin, FILE *newFile, char *libName){
	char line[1000], localFunction[16], functionName[25];
	while(1){
		memset(line,          '\0', 1000);
		memset(localFunction, '\0', 16  );
		memset(functionName,  '\0', 25  );

		if(fscanf(origin, "%15[^\n\t]", localFunction) == EOF) break;

		if(strcmp(localFunction, "local function ") == 0){
			fscanf(origin, "%25[^(\n]", functionName);

			if(functionName[0] >= 65 || functionName[0] <= 90){
				functionName[0] += 32; // A -> a
				fprintf(newFile, "%s.%s=function", libName, functionName);
			}
		}else{
			fprintf(newFile, "%s", localFunction);
		}
		
		if(fscanf(origin, "%1000[^\n]", line) != EOF) fprintf(newFile, "%s", line);
		fputc(fgetc(origin), newFile);
	}
}
/*
static void ???(FILE *origin, FILE *newFile){
	// lua reservad word
	char keyword[22][9] = {
		"and", "break", "do", "else", "elseif", "end", "false", "for", "function", "goto", "if",
		"in", "local", "nil", "not", "or", "repeat", "return", "then", "true", "until", "while",
	};

	// not special
	char valid[62] = {95}; // _
	for(int i = 48; i <=  57; i++) valid[i] = i; // 0-9
	for(int i = 65; i <=  90; i++) valid[i] = i; // A-Z
	for(int i = 97; i <= 122; i++) valid[i] = i; // a-z

	// character of comment; current; future
	char cm, cc, cf = fgetc(origin);

	// to "remove" them
	char comment[1000];

	// for break; comments lenght
	unsigned short fbreak = 0, comLen;

	while(cc != EOF){
		fileChar(&cc, &cf, origin);
		
		// space for tabelutations also 
		if(cc == '\n'){
			fputc(' ', newFile);
			
			fileChar(&cc, &cf, origin);
		}

		// commentaries (only push the "file point")
		if(cc == '-' && cf == '-'){
			fseek(origin, -2, SEEK_CUR);
			
			memset(comment, '\0', 1000);
			fscanf(origin, "%1000[^\n]", comment);

			fileChar(&cc, &cf, origin);
		}

		// print only letters and numbers characters
		for(int i = 0; i < 62; i++){
			for(int j = 0; j < 62; j++){
				if(((cc == ' ' && cf != EOF) || cc == valid[i]) && (cf == ' ' || cf == EOF || cf == valid[j])){
					fputc(cc, newFile);
					fbreak = 1;
					break;
				}
			}
			if(fbreak){
				fbreak = 0;
				break;
			}
		}
	}
}*/
