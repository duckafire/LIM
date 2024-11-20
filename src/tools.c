#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "heads.h"

bool t_strcmp2(char *str, char *v0, char *v1){
	if(str == NULL) return false;
	return (strcmp(str, v0) == 0 || strcmp(str, v1) == 0);
}

bool t_strcmp3(char *str0, char *str1){
	if(str0 == NULL && str1 == NULL)
		return true;

	if(str0 == NULL || str1 == NULL)
		return false;
	
	return (strcmp(str0, str1) == 0);
}

bool t_strcmp4(char *_str0, char *_str1){
	char *str0, *str1;
	bool result;

	str0 = t_rmvParen(_str0);
	str1 = t_rmvParen(_str1);

	result = (strcmp(str0, str1) == 0);

	free(str0);
	free(str1);
	return result;
}

FILE* t_copyFile(FILE *src, char *destName){
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

void t_copyAndExportFile(FILE *src){
	fclose(t_copyFile(src, lim.destineFileName));
}

char* t_rmvParen(char *word){
	unsigned short len = strlen(word);

	char *temp;
	temp = malloc(len + sizeof(char));
	strcpy(temp, word);

	if(temp[len - 1] == '(')
		temp[len - 1] = '\0';

	return temp;
}

unsigned int t_strlen2(char *str){
	if(str == NULL)
		return 0;

	return strlen(str);
}

void t_fcat(FILE *src, FILE *dest){
	char c;

	fseek(src,  0, SEEK_SET);
	fseek(dest, 0, SEEK_END);

	while((c = fgetc(src)) != EOF)
		fputc(c, dest);
}

long t_filelen(FILE *file){
	long cursor = ftell(file);
	fseek(file, 0, SEEK_END);

	long len = ftell(file);
	fseek(file, cursor, SEEK_SET);

	return len;
}

char* t_allocAndCopy(char *src){
	if(src == NULL)
		return NULL;

	char *dest;
	dest = malloc(strlen(src) + sizeof(char));
	strcpy(dest, src);

	return dest;
}

void t_buildStringFromFile(FILE *src, char *c, char **string){
	do{
		if(*c != '\n'){
			if(*c != '\0')
				mm_stringAdd(string, *c);
		}else{
			return;
		}
	}while((*c = fgetc(src)) != EOF);
}

char* t_setAnonyFuncName(unsigned short *index){
	char *dest;

	dest = malloc((sizeof(char) * 5) + INT_LEN(*index++));
	sprintf(dest, "__f%u_", *index);

	return dest;
}
