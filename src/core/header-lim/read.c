#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "../lim-global-variables.h"
#include "../tools/string-plus.h"
#include "../buf-man/buf-man.h"
#include "out-status.h"
#include "read.h"

static char c = 0;

#define FGETC (c = fgetc(lim.files.header_lim))
#define FPUTC(h, f) fputc(h, lim.header_partitions.f)
#define FSEEK fseek(lim.files.header_lim, -1, SEEK_CUR)

HF_OUT_STATUS read_header_file(char **indiv_part_status){
	if(!lim.flags.header_file)
		return HF_READING_DISABLE;

	if(lim.flags.until_stage != NULL)
		return HF_DISABLED_FLAG;

	lim.files.header_lim = fopen("header.lim", "r");
	if(lim.files.header_lim == NULL)
		return HF_HEADER_FILE_NOT_FOUND;

	if(fgetc(lim.files.header_lim) == EOF)
		return HF_HEADER_FILE_IS_EMPTY;
	FSEEK;


	HF_READ_STATUS status[4];
	start_reading(status);

	*indiv_part_status = malloc(5);

	for(short i = 0; i < 4; i++)
		(*indiv_part_status)[i] = '0' + status[i];

	(*indiv_part_status)[4] = '\0';


	return HF_CONTENT_READED;
}

static bool is_empty_file(FILE **file){
	if(*file == NULL)
		return true;

	fseek(*file, 0, SEEK_SET);

	if(fgetc(*file) == EOF){
		fclose(*file);
		*file = NULL;
		return true;
	}

	return false;
}

static bool is_empty_list(Queue **list){
	if(*list == NULL)
		return true;

	if((*list)->content[1] != NULL)
		return false;

	qee_free_queue(*list);
	*list = NULL;
	return true;
}

static HF_READ_STATUS refine_brute_status(FILE **buf, Queue **list, bool new_part_found, bool isfile){
	bool empty_status;

	if(isfile)
		empty_status = is_empty_file(buf);
	else
		empty_status = is_empty_list(list);


	if(!new_part_found && empty_status)
		return HF_EOF_FOUND;

	if(empty_status)
		return HF_EMPTY_PART;

	return HF_NEXT_PART;
}

static void start_reading(HF_READ_STATUS *refined_status){
	bool brute_status[4];

	for(short i = 0; i < 4; i++){
		brute_status[i] = false;
		refined_status[i] = HF_EOF_FOUND;
	}

	brute_status[0] = read_top_header();
	brute_status[1] = read_code_scope();
	brute_status[2] = read_list(&(lim.header_partitions.funct_list), brute_status[1]);
	brute_status[3] = read_list(&(lim.header_partitions.table_list), brute_status[2]);

	refined_status[0] = refine_brute_status(&(lim.header_partitions.top_header), NULL, brute_status[0], true);
	refined_status[1] = refine_brute_status(&(lim.header_partitions.code_scope), NULL, brute_status[1], true);
	refined_status[2] = refine_brute_status(NULL, &(lim.header_partitions.funct_list), brute_status[2], false);
	refined_status[3] = refine_brute_status(NULL, &(lim.header_partitions.table_list), brute_status[3], false);
}

static bool check_partitions_separator(void){
	// separator sequence: \n@\n
	if(c != '\n')
		return false;

	if(FGETC == '@'){

		if(FGETC == '\n'){
			FGETC; // jump last separator char.
			return true;
		}

		FSEEK;
	}

	FSEEK;
	return false;
}

static bool read_top_header(void){
	if(FGETC == EOF)
		return false;

	lim.header_partitions.top_header = tmpfile();

	do{
		if(check_partitions_separator())
			return true;

		FPUTC(c, top_header);
	}while(FGETC != EOF);

	return false;
}

static bool read_code_scope(void){
	if(c == EOF)
		return false;

	lim.header_partitions.code_scope = tmpfile();

	do{
		if(check_partitions_separator())
			return true;

		if(!isgraph(c)){
			while(FGETC != EOF && !isgraph(c))
				if(check_partitions_separator())
					return true;

			if(c == EOF)
				return false;
		}

		FPUTC(c, code_scope);
	}while(FGETC != EOF);

	return false;
}

static bool read_list(Queue **buf, bool is_this_found){
	if(c == EOF){
		if(is_this_found)
			*buf = qee_create(NULL, NULL);

		return false;
	}


	char *cur;
	string_set(&cur, STR_START);


	#define ADD_CUR                       \
		if(*buf == NULL)                  \
			*buf = qee_create(NULL, cur); \
		else                              \
			qee_add_item(buf, NULL, cur, false)
	//#enddef
	

	do{
		if(isgraph(c)){
			if(isalnum(c) || c == '_')
				string_add(&cur, c);

			continue;
		}


		if(cur[0] != '\0'){
			ADD_CUR;
			string_set(&cur, STR_RESTART);
		}


		if(check_partitions_separator()){
			string_set(&cur, STR_END);
			return true;
		}


		while(FGETC != EOF && !isgraph(c)){
			if(check_partitions_separator()){
				string_set(&cur, STR_END);
				return true;
			}
		}

		if(c == EOF){
			string_set(&cur, STR_END);
			return false;
		}

		FSEEK;
	}while(FGETC != EOF);

	if(cur[0] != '\0'){
		ADD_CUR;
	
	}else if(*buf == NULL){
		// set "EMPTY status" (it will be clean)
		*buf = qee_create(NULL, NULL);
	}

	string_set(&cur, STR_END);
	return false;
}
