#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include "tools/lim-global-variables.h"
#include "tools/string-plus.h"
#include "process-master.h"
#include "check-content/check-content.h"
#include "treat.h"
#include "ident-man.h"

static char c;

void read_source_file(void){
	char *tmp = NULL;

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
	free_nickname_buffers();
	string_set(&tmp, STR_END);
	build_destine_file();
}

static void get_and_put_from_buffer(FILE *buf, bool separator){
	if(buf == NULL)
		return;

	fseek(buf, 0, SEEK_SET);

	while( (c = fgetc(buf)) != EOF )
		fputc(c, lim.files.destine);

	if(separator)
		fputc(';', lim.files.destine);
}

static void build_destine_file(void){
	lim.files.destine = fopen(lim.files.destine_name, "w");

	get_and_put_from_buffer(lim.header_partitions.top_header, false);
	fprintf(lim.files.destine, "\nlocal _={}\ndo ");

	get_and_put_from_buffer(lim.header_partitions.code_scope,    true);
	get_and_put_from_buffer(lim.buffers.root.scope_func_pointer, false);
	get_and_put_from_buffer(lim.buffers.root.scope_func_address, true);
	get_and_put_from_buffer(lim.buffers.root.scope_var_tab,      true);

	get_and_put_from_buffer(lim.buffers.destine_file, false);
	fprintf(lim.files.destine, " end\nlocal lim=_");
}
