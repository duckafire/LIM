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
#define FSEEK fseek(lim.files.source, -1, SEEK_CUR)

bool is_identifier(char c){
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
		if(is_from_lua(table, lua_std_table))
			treat_lua_std_table(table, buf);

		else if(is_from_header(table, lim.header_partitions.table_list))
			treat_header_table(table, buf);

		else
			treat_source_file_ident(table, buf);

	}else{
		if(is_from_lua(buf, lua_kw))
			treat_const(&buf);

		else if(is_from_lua(buf, lua_std_func))
			treat_lua_std_func(buf);

		else if(is_from_lua(buf, lua_std_table))
			treat_lua_std_table(buf, NULL);

		else if(is_from_header(buf, lim.header_partitions.funct_list))
			treat_header_func(buf);

		else if(is_from_header(buf, lim.header_partitions.table_list))
			treat_header_table(buf, NULL);

		else
			treat_source_file_ident(buf, NULL);
	}


	FSEEK;
	string_set(&buf, STR_END);
	string_set(&table, STR_END);
	return true;
}

static bool is_from_lua(char *ident, const char *array[]){
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
