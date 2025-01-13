#include <stdbool.h>
#include <ctype.h>
#include "../lim-global-variables.h"
#include "process-master.h"
#include "check-content/check-content.h"
#include "treat.h"

void read_source_file(void){
	char c, *tmp = NULL;

	while((c = fgetc(lim.files.source)) != EOF){
		if(clear_white_spaces(&c))
			break;

		if(tmp != NULL)
			treat_const(&tmp);
		
		// NOTE: single from "ident.h"
		// send its content to `treat.c`
		if(is_identifier(c))
			continue;

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
	build_destine_file();
}

static void build_destine_file(void){}
