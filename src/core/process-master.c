#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include "tools/lim-global-variables.h"
#include "tools/string-plus.h"
#include "tools/verbose.h"
#include "process-master.h"
#include "check-content.h"
#include "treat.h"
#include "ident-man.h"

static char c;

void read_source_file(void){
	pverbose(V_START_PROCESS, "Read input file");

	char *tmp = NULL;

	lim.files.source = fopen(lim.files.source_name, "r");
	lim.buffers.destine_file = tmpfile();

	start_nickname_buffers();

	while((c = fgetc(lim.files.source)) != EOF || tmp != NULL){
		if(clear_white_spaces(&c))
			if(tmp == NULL)
				break;

		if(tmp != NULL){
			pverbose(V_CONST_FOUND, tmp);
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

	pverbose(V_CLOSE_PROCESS, "Read input file");
}

static void get_and_put_from_buffer(FILE *buf, bool separator, char *its_name, char *verbose_msg){
	if(buf == NULL){
		if(its_name != NULL)
			pverbose(V_WARNING, its_name, "is empty, none content were found for it.", NULL);

		return;
	}

	pverbose(V_INSERTING, verbose_msg);
	fseek(buf, 0, SEEK_SET);

	while( (c = fgetc(buf)) != EOF )
		fputc(c, lim.files.destine);

	if(separator)
		fputc(';', lim.files.destine);
}

void build_destine_file(void){
	pverbose(V_START_PROCESS, "Build output file");

	lim.files.destine = fopen(lim.files.destine_name, "w");

	get_and_put_from_buffer(lim.header_partitions.top_header, false, "'Top header'", "'Top header' partition, from 'header.lim'");

	pverbose(V_INSERTING, "Library table + start of 'do end' block");
	if((lim.header_partitions.top_header != NULL))
		fputc('\n', lim.files.destine);

	fprintf(lim.files.destine, "local _={}\ndo ");

	get_and_put_from_buffer(lim.header_partitions.code_scope,    true,  "'Code scope'", "'Code scope' partition, from 'header.lim'");
	get_and_put_from_buffer(lim.buffers.root.scope_func_pointer, false, "Functions scope", "Functions, from Lua and 'header.lim', that were used");
	get_and_put_from_buffer(lim.buffers.root.scope_func_address, true,  NULL, NULL);

	get_and_put_from_buffer(lim.buffers.destine_file, false, "Compacted content", "Compacted content from input file");

	pverbose(V_INSERTING, "End of 'do end' block + reference to library table");
	fprintf(lim.files.destine, " end\nlocal %s=_", lim.flags.lib_name);

	pverbose(V_CLOSE_PROCESS, "Build output file");
}
