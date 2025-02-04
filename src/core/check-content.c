#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "tools/lim-global-variables.h"
#include "tools/string-plus.h"
#include "tools/queue.h"
#include "treat.h"
#include "check-content.h"

// NOTE: Divided in:
// // constants
// // identifiers

///// CONSTANTS /////

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

		if(lastc_is_alnum && space && (c == '_' || isalnum(c)))
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

///// IDENTIFIERS /////

static const char *lua_kw[]={"do","if","in","or","and","end","for","nil","not","else","then","true","break","false","local","until","while","elseif","repeat","return","function", NULL};
static const char *lua_std_func[]={"print","tonumber","tostring","ipairs","type","pairs","assert","error","setmetatable","getmetatable","rawset","rawget","rawlen","rawequal","pcall","xpcall","select","next","collectgarbage","require","load","dofile","loadfile",NULL};
static const char *lua_std_table[]={"courotine","debug","io","math","os","package","string","table","utf8",NULL};

#define FGETC (c = fgetc(lim.files.source))

bool is_identifier(char c, char **tmp){
	if(!isalpha(c) && c != '_')
		return false;

	char *buf, *ident, *table_key;

	ident = table_key = NULL;
	string_set(&buf, STR_START);


	do{
		if(ident == NULL && strchr(".:", c) != NULL){
			ident = string_copy(buf);
			string_set(&buf, STR_RESTART);
			string_add(&buf, c);
			continue;
		}

		string_add(&buf, c);
	}while(FGETC != EOF && (isalnum(c) || strchr("_.:", c) != NULL));

	if(ident == NULL)
		ident = buf;
	else
		table_key = buf;


	if(table_key != NULL){
		if(is_from_lua(ident, CKIA_LUA_STD_TABLE))
			treat_standard_from(true, ident, table_key, &(lim.buffers.root.table_from_lua));

		else if(is_from_header(ident, lim.header_partitions.table_list))
			treat_standard_from(true, ident, table_key, &(lim.buffers.root.table_from_header));

		else
			treat_ident(ident, table_key);

	}else{
		if(is_from_lua(ident, CKIA_LUA_KW))
			*tmp = string_copy(ident); // it will be treat in "read_source_file"

		else if(is_from_lua(ident, CKIA_LUA_STD_FUNC))
			treat_standard_from(true, ident, NULL, &(lim.buffers.root.func_from_lua));

		else if(is_from_lua(ident, CKIA_LUA_STD_TABLE))
			treat_standard_from(true, ident, NULL, &(lim.buffers.root.table_from_lua));

		else if(is_from_header(ident, lim.header_partitions.funct_list))
			treat_standard_from(true, ident, NULL, &(lim.buffers.root.func_from_header));

		else if(is_from_header(ident, lim.header_partitions.table_list))
			treat_standard_from(true, ident, NULL, &(lim.buffers.root.table_from_header));

		else
			treat_ident(ident, NULL);
	}


	if(c != EOF)
		fseek(lim.files.source, -1, SEEK_CUR);

	string_set(&ident, STR_END);
	string_set(&table_key, STR_END);
	return true;
}

bool is_from_lua(char *ident, CKIA_ID id){
	const char **array;

	switch(id){
		case CKIA_LUA_KW:        array = lua_kw;        break;
		case CKIA_LUA_STD_FUNC:  array = lua_std_func;  break;
		case CKIA_LUA_STD_TABLE: array = lua_std_table; break;
	}

	for(short i = 0; array[i] != NULL; i++)
		if(strcmp(ident, array[i]) == 0)
			return true;

	return false;
}

static bool is_from_header(char *ident, Queue *buf){
	Queue *p;

	for(p = buf; p != NULL; p = p->next)
		if(strcmp(ident, p->ident) == 0)
			return true;

	return false;
}
