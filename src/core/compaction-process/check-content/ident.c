#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "../../lim-global-variables.h"
#include "../../string-plus.h"
#include "../../buf-man/buf-man.h"
#include "../treat.h"
#include "check-content.h"

static const char *lua_kw[]={"do","if","in","or","and","end","for","nil","not","else","then","true","break","false","local","until","while","elseif","repeat","return","function", NULL};
static const char *lua_std_func[]={"print","tonumber","tostring","ipairs","type","pairs","assert","error","setmetatable","getmetatable","rawset","rawget","rawlen","rawequal","pcall","xpcall","select","next","collectgarbage","require","load","dofile","loadfile",NULL};
static const char *lua_std_table[]={"courotine","debug","io","math","os","package","string","table","utf8",NULL};

#define FGETC (c = fgetc(lim.files.source))

bool is_identifier(char c, char **tmp){
	if(!isalpha(c) && c != '_')
		return false;

	char *buf;
	char *table = NULL;

	string_set(&buf, STR_START);


	do{
		if(table == NULL && (c == '.' || c == ':')){
			table = string_copy(buf);
			string_set(&buf, STR_RESTART);
			continue;
		}

		string_add(&buf, c);
	}while(FGETC != EOF && (isalnum(c) || c == '_' || c == '.' || c == ':'));


	if(table != NULL){
		if(is_from_lua(table, CKIA_LUA_STD_TABLE))
			putchar(0);//treat_std_hdr_ident(table, buf, false, true);

		else if(is_from_header(table, lim.header_partitions.table_list))
			putchar(0);//treat_std_hdr_ident(table, buf, false, false);

		else
			treat_ident(table, buf);

	}else{
		if(is_from_lua(buf, CKIA_LUA_KW))
			*tmp = string_copy(buf);

		else if(is_from_lua(buf, CKIA_LUA_STD_FUNC))
			putchar(0);//treat_std_hdr_ident(buf, NULL, true, true);

		else if(is_from_lua(buf, CKIA_LUA_STD_TABLE))
			putchar(0);//treat_std_hdr_ident(buf, NULL, false, true);

		else if(is_from_header(buf, lim.header_partitions.funct_list))
			putchar(0);//treat_std_hdr_ident(buf, NULL, true, false);

		else if(is_from_header(buf, lim.header_partitions.table_list))
			putchar(0);//treat_std_hdr_ident(buf, NULL, false, false);

		else
			treat_ident(buf, NULL);
	}


	if(c != EOF)
		fseek(lim.files.source, -1, SEEK_CUR);
	string_set(&buf, STR_END);
	string_set(&table, STR_END);
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
		if(strcmp(ident, p->content[1]) == 0)
			return true;

	return false;
}
