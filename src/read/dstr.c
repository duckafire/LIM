#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "head.h"

static char* curWord;

void dstr_init(void){
	curWord = malloc(1);
	curWord[0] = '\0';
}

void dstr_addc(char c){
	UNINT len = strlen(curWord) + 3;

	char* temp;
	temp = malloc(len);

	memset(temp, '\0', len);
	strcpy(temp, curWord);
	temp[len - 3] = c;

	free(curWord);
	curWord = temp;
}

void dstr_fputs(void){
	FILE *output = fopen("output.lim", "w");
	fputs(curWord, output);
	fclose(output);

	dstr_end();
	dstr_init();
}

void dstr_end(void){
	free(curWord);
	curWord = NULL;
}
