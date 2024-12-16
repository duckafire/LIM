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

unsigned int t_strlen2(char *str){
	if(str == NULL)
		return 0;

	return strlen(str);
}

void t_fcat(FILE *dest, FILE *src){
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

void t_getStringFromFile(FILE *src, char *c, char **string){
	do{
		if(*c != '\n'){
			mm_stringAdd(string, *c);
		}else{
			return;
		}
	}while((*c = fgetc(src)) != EOF);
}

char* t_setAnonyFuncName(unsigned short *index){
	char *dest;

	dest = malloc((sizeof(char) * 5) + INT_LEN(*index));
	sprintf(dest, "__f%u_", *index);
	(*index)++;

	return dest;
}

char* t_getLongFlag(char *f, char *flag){
	return flag;
}
