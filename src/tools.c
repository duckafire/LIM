#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "heads.h"

bool tools_strcmp2(char *str, char *v0, char *v1){
	if(str == NULL) return false;
	return (strcmp(str, v0) == 0 || strcmp(str, v1) == 0);
}

bool tools_strcmp3(char *str0, char *str1){
	if(str0 == NULL || str1 == NULL)
		return false;
	return (strcmp(str0, str1) == 0);
}

FILE* tools_copyFile(FILE *src, char *destName){
	char c = 0;
	FILE *dest;

	// use `destName == NULL` for
	// temporary buffers
	if(destName == NULL)
		dest = tmpfile();
	else
		dest = fopen(destName, "w");
	
	fseek(src, 0, SEEK_SET);

	// copy content
	while((c = fgetc(src)) != EOF)
		fputc(c, dest);

	fseek(src,  0, SEEK_SET);
	fseek(dest, 0, SEEK_SET);
	return dest;
}

char* tools_rmvParen(char *word){
	unsigned short len = strlen(word);

	char *temp;
	temp = malloc(len + 1);
	strcpy(temp, word);

	if(temp[len - 1] == '(')
		temp[len - 1] = '\0';

	return temp;
}

unsigned short tools_strlen2(char *str){
	if(str == NULL)
		return 0;

	return strlen(str);
}

void tools_fcat(FILE *src, FILE *dest){
	char c;

	fseek(src,  0, SEEK_SET);
	fseek(dest, 0, SEEK_END);

	while((c = fgetc(src)) != EOF)
		fputc(c, dest);
}

void tools_initDinStr(char **buf){
	*buf = malloc(sizeof(char));
	*buf[0] = '\0';
}

void tools_addDinStr(char *buf, char c){
	unsigned short len = strlen(buf);

	char *tmp;
	tmp = malloc(len + 1);
	strcpy(tmp, buf);

	free(buf);
	buf = malloc(len + 2);

	strcpy(buf, tmp);
	buf[len] = c;
	buf[len + 1] = '\0';

	free(tmp);
}

void tools_endDinStr(char **buf, bool restart){
	free(*buf);
	*buf = NULL;

	if(restart)
		tools_initDinStr(buf);
}

long tools_filelen(FILE *file){
	long cursor = ftell(file);
	fseek(file, 0, SEEK_END);

	long len = ftell(file);
	fseek(file, cursor, SEEK_SET);

	return len;
}
