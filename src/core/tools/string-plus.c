#include <stdlib.h>
#include <string.h>
#include "string-plus.h"

void string_set(char **str, STR mode){
	if(mode != STR_START)
		free(*str);

	if(mode != STR_END){
		*str = malloc(sizeof(char));
		*str[0] = '\0';
	}
}

char* string_copy(char *str){
	if(str == NULL)
		return NULL;

	char *buf;

	buf = malloc(strlen(str) + sizeof(char));
	strcpy(buf, str);

	return buf;
}

void string_add(char **str, char c){
	const unsigned short len = strlen(*str);
	char *buf;

	buf = *str;

	*str = malloc(len + 2);
	strcpy(*str, buf);

	*str[len] = c;
	*str[len + 1] = '\0';

	free(buf);
}
