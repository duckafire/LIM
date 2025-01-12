#include <stdbool.h>
#include <ctype.h>
#include "../lim-global-variables.h"
#include "process-master.h"
#include "check-content/const.h"
#include "check-content/ident.h"

// debug
#include <stdio.h>
#include "../tools/string-plus.h"
static void printfc(char **s){printf("C - %s\n", *s); string_set(s, STR_END); *s = NULL;}
static void printfi(char **s){printf("I -- %s\n", *s); string_set(s, STR_END); *s = NULL;}

void read_source_file(void){
	char c, *tmp = NULL;

	while((c = fgetc(lim.files.source)) != EOF){
		if(clear_white_spaces(&c))
			break;

		if(tmp != NULL)
			printfc(&tmp);
		
		if(is_identifier(c, &tmp)){
			if(!is_lua_keyword(tmp)) // TODO: it will be static
				printfi(&tmp);

			continue;
		}

		if(is_number(c, &tmp))
			continue;

		if(is_string(c, &tmp))
			continue;

		if(is_table_env(c, &tmp))
			continue;

		if(is_commentary(c))
			continue;

		is_special_char(c, &tmp);
	}

	string_set(&tmp, STR_END);
}
