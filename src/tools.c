#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "heads.h"

bool tools_strcmp2(char *str, char *v0, char *v1){
	if(str == NULL) return false;
	return (strcmp(str, v0) == 0 || strcmp(str, v1) == 0);
}

FILE* tools_copyFile(FILE *src, char *destName){
	char c = 0;
	FILE *dest;
	const bool cloneBuf = (destName == NULL);

	// use `destName == NULL` for
	// temporary buffers
	if(cloneBuf)
		dest = tmpfile();
	else
		dest = fopen(destName, "w");
	
	fseek(src, 0, SEEK_SET);

	// copy content
	while(fread(&c, sizeof(char), 1, src) > 0 && c != EOF){
		if(cloneBuf)
			fwrite(&c, sizeof(c), 1, dest);
		else
			fprintf(dest, "%c", c);
	}

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

char* tools_newStrNoParen(char *_str){
	unsigned short len = strlen(_str);
	char *str;

	str = malloc(len + 1);
	strcpy(str, _str);

	if(str[len - 1] == '(')
		str[len - 1] = '\0';

	return str;
}

bool tools_strcmpNoParen(char *_str0, char *_str1){
	if(_str0 == NULL || _str1 == NULL)
		return false;

	char *str0;
	char *str1;

	str0 = tools_newStrNoParen(_str0);
	str1 = tools_newStrNoParen(_str1);

	bool result = (strcmp(str0, str1) == 0);

	free(str0);
	free(str1);
	return result;

}
