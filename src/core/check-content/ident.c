#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "../tools/lim-global-variables.h"
#include "../tools/string-plus.h"
#include "../tools/queue.h"
#include "../treat.h"
#include "check-content.h"

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
		treat_ident(ident, table_key);
		//if(is_from_lua(ident, CKIA_LUA_STD_TABLE))
		//	putchar(0);

		//else if(is_from_header(ident, lim.header_partitions.table_list))
		//	putchar(0);

		//else
		//	treat_ident(ident, table_key);

	}else{
		if(is_from_lua(ident, CKIA_LUA_KW))
			*tmp = string_copy(ident); // it will be treat in "read_source_file"
		else
			treat_ident(ident, NULL);

		//else if(is_from_lua(ident, CKIA_LUA_STD_FUNC))
		//	putchar(0);

		//else if(is_from_lua(ident, CKIA_LUA_STD_TABLE))
		//	putchar(0);

		//else if(is_from_header(ident, lim.header_partitions.funct_list))
		//	putchar(0);

		//else if(is_from_header(ident, lim.header_partitions.table_list))
		//	putchar(0);

		//else
		//	treat_ident(ident, NULL);
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
