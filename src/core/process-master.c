#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include "tools/lim-global-variables.h"
#include "tools/string-plus.h"
#include "process-master.h"
#include "check-content/check-content.h"
#include "treat.h"
#include "ident-man.h"

void read_source_file(void){
	char c, *tmp = NULL;

	lim.buffers.destine_file = tmpfile();
	start_nickname_buffers();

	while((c = fgetc(lim.files.source)) != EOF || tmp != NULL){
		if(clear_white_spaces(&c))
			if(tmp == NULL)
				break;

		if(tmp != NULL){
			treat_const(tmp);

			string_set(&tmp, STR_END);

			if(c == EOF)
				break;
		}
		
		// NOTE: single from "ident.h"
		// send its content to `treat.c`
		if(is_identifier(c, &tmp))
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

	finish_treatment();
	free_nickname_buffers(true);
	string_set(&tmp, STR_END);
	build_destine_file();
}

static void build_destine_file(void){
	char c;

	fseek(lim.buffers.destine_file, 0, SEEK_SET);
	
	while( (c = fgetc(lim.buffers.destine_file)) != EOF)
		putchar(c);

	putchar('\n');
}
