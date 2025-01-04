#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "../../master/lim-global-variables.h"
#include "../tools/string-plus.h"
#include "../buf-man/buf-man.h"
#include "out-status.h"
#include "read.h"

static char c;
static bool clear_white_spaces_status;

#define FGETC (c = fgetc(lim.files.header_lim))
#define FPUTC(h, f) fputc(h, lim.header_partitions.f)
#define FSEEK fseek(lim.files.header_lim, -1, SEEK_CUR);
#define IS_PART_EMPTY                            \
	if(c == EOF || check_partitions_separator()) \
		return HF_EMPTY_PART

HF_OUT_STATUS read_header_file(char **indiv_part_status){
	if(!lim.flags.header_file)
		return HF_READING_DISABLE;

	if(lim.flags.until_stage != NULL)
		return HF_DISABLED_FLAG;

	lim.files.header_lim = fopen("header.lim", "r");
	if(lim.files.header_lim == NULL)
		return HF_HEADER_FILE_NOT_FOUND;


	HF_READ_STATUS status[4];
	start_reading(status);

	*indiv_part_status = malloc(5);

	for(short i = 0; i < 4; i++)
		(*indiv_part_status)[i] = '0' + status[i];

	(*indiv_part_status)[4] = '\0';


	return HF_CONTENT_READED;
}

static bool check_partitions_separator(void){
	// separator sequence: \n@\n
	if(c != '\n')
		return false;

	if(FGETC == '@'){
		if(FGETC == '\n')
			return true;

		FSEEK;
	}

	FSEEK;
	return false;
}

static bool clear_white_spaces(char h, FILE *dest){
	if(isgraph(c)){
		clear_white_spaces_status = false;
		return false;
	}

	while(FGETC != EOF && !isgraph(c));

	if(c == EOF)
		return true;

	if(dest != NULL)
		fputc(h, dest);

	clear_white_spaces_status = true;
	return false;
}

static void start_reading(HF_READ_STATUS *status){
	for(short i = 0; i < 4; i++)
		status[i] = HF_EOF_FOUND;

	status[0] = read_top_header();

	if(status[0] != HF_EOF_FOUND)
		status[1] = read_code_scope();

	if(status[1] != HF_EOF_FOUND)
		status[2] = read_list(&(lim.header_partitions.funct_list));

	if(status[2] != HF_EOF_FOUND)
		status[3] = read_list(&(lim.header_partitions.table_list));
}

static HF_READ_STATUS read_top_header(void){
	IS_PART_EMPTY;

	lim.header_partitions.top_header = tmpfile();

	while(FGETC != EOF){
		if(check_partitions_separator())
			return HF_NEXT_PART;

		FPUTC(c, top_header);
	}

	return HF_EOF_FOUND;
}

static HF_READ_STATUS read_code_scope(void){
	clear_white_spaces('\0', NULL);
	IS_PART_EMPTY;

	lim.header_partitions.code_scope = tmpfile();

	while(FGETC != EOF){
		if(check_partitions_separator())
			return true;

		if(clear_white_spaces(' ', lim.header_partitions.code_scope))
			return false;

		FPUTC(c, code_scope);
	}

	return false;
}

static HF_READ_STATUS read_list(Queue **buf){
	clear_white_spaces('\0', NULL);
	IS_PART_EMPTY;

	char *cur;
	string_set(&cur, STR_START);

	while(FGETC != EOF){
		if(check_partitions_separator()){
			string_set(&cur, STR_END);
			return HF_NEXT_PART;
		}

		if(clear_white_spaces('\0', NULL)){
			string_set(&cur, STR_END);
			return HF_EOF_FOUND;
		}

		if(clear_white_spaces_status){
			if(*buf == NULL)
				*buf = qee_create(NULL, cur);
			else
				qee_add_item(buf, NULL, cur, false);

			string_set(&cur, STR_RESTART);
			continue;
		}

		string_add(&cur, c);
	}

	string_set(&cur, STR_END);
	return HF_EOF_FOUND;
}
