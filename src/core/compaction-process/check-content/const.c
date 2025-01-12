#include <stdbool.h>
#include <ctype.h>
#include "../../lim-global-variables.h"
#include "../../tools/string-plus.h"
#include "const.h"

#define FGETC (c = fgetc(lim.files.source))
#define FSEEK fseek(lim.files.source, -1, SEEK_CUR)

bool clear_white_spaces(char *c){
	while(!isgraph(*c) && (*c = fgetc(lim.files.source)) != EOF);

	return (*c == EOF);
}

bool is_number(char c, char **tmp){
	if(!isdigit(c))
		return false;

	string_set(tmp, STR_START);

	do{
		string_add(tmp, c);
	}while(FGETC != EOF && (isalnum(c) || c == '_' || c == '.'));

	FSEEK;
	return true;
}

static void build_alnum_string(char c, char **tmp){
}

bool is_string(char c, char **tmp){
	if(c != '\'' && c != '"')
		return false;

	const char sign = c;
	bool inverted_bar;

	string_set(tmp, STR_START);
	string_add(tmp, sign);

	while(FGETC != EOF){
		string_add(tmp, c);

		if(!inverted_bar){
			if(c == '\\')
				inverted_bar = true;
			else if(c == sign)
				break;
		}else{
			inverted_bar = false;
		}
	}

	return true;
}

bool is_table_env(char c, char **tmp){
	if(c != '{')
		return false;

	unsigned short layer = 0;

	string_set(tmp, STR_START);
	string_add(tmp, '{');

	while(FGETC != EOF){
		if(clear_white_spaces(&c))
			break;

		string_add(tmp, c);

		if(c == '{'){
			layer++;

		}else if(c == '}'){
			if(layer > 0)
				layer--;

			if(layer == 0)
				break;
		}
	}

	return true;
}

bool is_commentary(char c){
	if(c != '-')
		return false;

	if(FGETC == '-'){
		if(is_commentary_block(c))
			return true;

		return clear_single_line_commentary(c);
	}

	FSEEK;
	return false;
}

static bool is_commentary_block(char c){
	for(short i = 0; i < 2; i++){
		if(FGETC != '[' || c == EOF){
			FSEEK;
			return false;
		}
	}

	while(FGETC != EOF){
		if(c == ']'){
			if(FGETC == EOF || c == ']')
				break;
		}
	}

	return true;
}

static bool clear_single_line_commentary(char c){
	while(c != '\n' && FGETC != EOF);

	return true;
}

void is_special_char(char c, char **tmp){
	string_set(tmp, STR_START);
	string_add(tmp, c);
}
