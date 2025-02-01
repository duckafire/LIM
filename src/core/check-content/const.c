#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "../tools/lim-global-variables.h"
#include "../tools/string-plus.h"
#include "check-content.h"

#define FGETC (c = fgetc(lim.files.source))
#define FSEEK fseek(lim.files.source, -1, SEEK_CUR)

bool clear_white_spaces(char *c){
	// this, if true, indicate that
	// a word need to be writed
	if(*c == EOF)
		return false;

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

	if(c != EOF)
		FSEEK;
	return true;
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
	bool lastc_is_alnum = false, space = false;

	string_set(tmp, STR_START);
	string_add(tmp, '{');

	while(FGETC != EOF){
		space = !isgraph(c);

		if(clear_white_spaces(&c))
			break;
		else if(is_commentary(c))
			continue;

		if(lastc_is_alnum && space && isalnum(c))
			string_add(tmp, ' ');
		string_add(tmp, c);

		if(c == '{'){
			layer++;

		}else if(c == '}'){
			if(layer > 0)
				layer--;
			else
				break;
		}

		lastc_is_alnum = (c == '_' || isalnum(c));
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

	if(c != EOF)
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

	if(c == ',')
		return;

	if(c == '='){
		FGETC;

		if(c == EOF){
			return;

		}else if(strchr("=><", c) == NULL){
			FSEEK;
			return;
		}

		FSEEK;
	}

	while(FGETC != EOF){
		if(clear_white_spaces(&c))
			break;

		if(isalnum(c) || strchr("_'\"{,", c) != NULL)
			break;

		if(c == '-'){
			if(FGETC == '-'){
				fseek(lim.files.source, -2, SEEK_CUR);
				return;
			}
			c = '-';
			FSEEK;
		}

		string_add(tmp, c);
	}

	if(c != EOF)
		FSEEK;
}
