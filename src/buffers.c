#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "heads.h"

static FILE *collect;
static char *ident; // identifier

// COLLECT
void collect_init(void){
	collect = tmpfile();
}

void collect_add(char c){
	fwrite(&c, sizeof(char), 1, collect);
}

FILE* collect_get(void){
	return collect;
}

void collect_end(void){
	fclose(copyFile(collect, "output.lim"));
	fclose(collect);
}

// IDENTIFIER
void ident_init(void){
	ident = malloc(1);
	ident[0] = '\0';
}

void ident_add(char c){
	unsigned int len = strlen(ident) + 3;

	char *temp;
	temp = malloc(len);

	memset(temp, '\0', len);
	strcpy(temp, ident);
	temp[len - 3] = c;

	free(ident);
	ident = temp;
}

char* ident_get(void){
	return ident;
}

void ident_end(short restart){
	free(ident);
	ident = NULL;

	if(restart)
		ident_init();
}
