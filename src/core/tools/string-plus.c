#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "string-plus.h"

void string_set(char **str, STR mode){
	if(mode != STR_START){
		free(*str);
		*str = NULL;
	}

	if(mode != STR_END){
		*str = malloc(sizeof(char));
		*str[0] = '\0';
	}
}

void string_add(char **str, char c){
	const unsigned short len = strlen(*str);
	char *buf;

	buf = *str;

	*str = malloc(len + 2);
	strcpy(*str, buf);

	(*str)[len] = c;
	(*str)[len + 1] = '\0';

	free(buf);
}

char* string_copy(char *str){
	if(str == NULL)
		return NULL;

	char *buf;

	buf = malloc(strlen(str) + sizeof(char));
	strcpy(buf, str);

	return buf;
}

bool string_compare(char *str0, char *str1){
	if(str0 == NULL && str1 == NULL)
		return true;

	if(str0 == NULL || str1 == NULL)
		return false;

	return (strcmp(str0, str1) == 0);
}

short string_length(char *str){
	if(str == NULL)
		return -1;

	return strlen(str);
}
