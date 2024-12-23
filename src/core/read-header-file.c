#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "../interface/lim-global-variables.h"
#include "read-header-file.h"

static char c;
static FILE *header_file;

#define FGETC (c = fgetc(header_file))

void get_header_file_content(void){
	if(is_enable_header_file() != GET_SUCCESS)
		return;

	if(get_content_from_top_header() != NEXT)
		return;

	if(get_content_from_code_scope() != NEXT)
		return;

	get_content_from_funct_list();
}

static header_use_state is_enable_header_file(void){
	if(!lim.flags.header_file)
		return GET_FAIL;

	if(lim.flags.until_stage != 0)
		return GET_FAIL;

	header_file = fopen("header.lim", "r");
	if(header_file == NULL)
		return GET_FAIL;

	return GET_SUCCESS;
}

static bool is_partition_separator(void){
	if(c == '\n')
		return false;

	if(FGETC == '@' && c != EOF){
		if(FGETC == '\n' && c != EOF)
			return true;

		if(c != EOF)
			fseek(header_file, -1, SEEK_CUR);
	}

	if(c != EOF)
		fseek(header_file, -1, SEEK_CUR);

	return false;
}

#define GET_START                 \
	if(is_partition_separator()) \
		return EMPTY

#define IS_EMPTY                        \
	while(!isgraph(FGETC) && c != EOF){ \
		if(is_partition_separator())    \
			return EMPTY;               \
	}                                   \
	if(c == EOF)                        \
		return EMPTY

#define GET_FINAL         \
	if(c == EOF)          \
		return EOF_FOUND; \
	return NO_NEXT

static header_partition_state get_content_from_top_header(void){
	GET_START;

	while(FGETC != EOF){
		if(is_partition_separator())
			return NEXT;

		fputc(c, lim.header_partitions.top_header);
	}

	GET_START;
}

static header_partition_state get_content_from_code_scope(void){
	GET_START;
	IS_EMPTY;

	while(FGETC != EOF){
		if(!isgraph(c)){
			while(!isgraph(c) && c != EOF);

			if(c == EOF)
				return NO_NEXT;

			fputc(' ', lim.header_partitions.code_scope);
		}

		if(is_partition_separator())
			return NEXT;

		fputc(c, lim.header_partitions.code_scope);
	}

	GET_FINAL;
}

static header_partition_state get_content_from_funct_list(void){
	GET_START;
	IS_EMPTY;

	while(FGETC != EOF){
		if(!isgraph(c)){
			while(!isgraph(c) && c != EOF);

			if(c == EOF)
				return NO_NEXT;
		}

		while((isalpha(c) || c == '_') && c != EOF)
			fputc(c, lim.header_partitions.funct_list);

		if(c == EOF)
			return NO_NEXT;

		fseek(header_file, -1, SEEK_CUR);
		fputc('\n', lim.header_partitions.funct_list);
	}

	GET_FINAL;
}

#undef FGETC
#undef GET_START
#undef IS_EMPTY
#undef GET_FINAL
