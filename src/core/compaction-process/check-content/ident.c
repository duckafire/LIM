#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "../../lim-global-variables.h"
#include "../../tools/string-plus.h"
#include "ident.h"

static const char *lua_kw[] = {"do","if","in","or","and","end","for","nil","not","else","then","true","break","false","local","until","while","elseif","repeat","return","function", NULL};

#define FGETC (c = fgetc(lim.files.source))
#define FSEEK fseek(lim.files.source, -1, SEEK_CUR)

bool is_identifier(char c, char **tmp){
	if(!isalpha(c) && c != '_')
		return false;

	string_set(tmp, STR_START);

	do{
		// TODO: add "magic" here

		string_add(tmp, c);
	}while(FGETC != EOF && (isalnum(c) || c == '_' || c == '.' || c == ':'));


	FSEEK;
	return true;
}

// TODO: it will be static
bool is_lua_keyword(char *ident){
	for(short i = 0; lua_kw[i] != NULL; i++)
		if(strcmp(ident, lua_kw[i]) == 0)
			return true;

	return false;
}
