#include <stdio.h>
#include <string.h>
#include "flags/cmp.h"
#include "flags/list.h"
#include "fatal/error.h"
#include "fatal/info.h"
#include "help.h"
#include "process-master.h"
#include "check.h"
#include "treat.h"

#define IS_NEXT_ARG_NULL \
	(i == args.index)

#define OTHER_ARGS_ARE_IGNORED(n) \
	if(args.index > n)            \
		printf("[LIM] Arguments above %dth were ignored.\n\n", n)

#define CHECK_FLAG_REPETITION(cond)            \
	if(cond){                                  \
		ERROR_flag_repetititon(args.value[i]); \
	}

// CaS => Check and Set
#define CaS_CHAR_FLAG(dest, set, ...)              \
	if(flag_cmp(args.value[i], __VA_ARGS__)){                \
		CHECK_FLAG_REPETITION( (dest != NULL) )              \
		if(IS_NEXT_ARG_NULL){                                \
			ERROR_suffix_expected_after_flag(args.value[i]); \
		}                                                    \
		set(args.value[++i]);                                \
		continue;                                            \
	}

#define CaS_BOOL_FLAG(cond, var, val, ...) \
	if(flag_cmp(args.value[i], __VA_ARGS__)){        \
		CHECK_FLAG_REPETITION( (cond) )              \
		var = val;                                   \
		continue;                                    \
	}

void is_it_information_flag(void){
	if(args.index == 0){
		no_arguments_to_lim;
	}

	if(flag_cmp(args.value[1], FLAG_VERSION)){
		OTHER_ARGS_ARE_IGNORED(1);

		print_current_version(args.program_version);
	}

	if(flag_cmp(args.value[1], FLAG_HELP)){
		if(args.index == 1)
			show_help_messages(NULL);

		OTHER_ARGS_ARE_IGNORED(2);

		show_help_messages(args.value[2]);
	}
}

void search_and_set_source_file(void){
	FILE *tmp;

	args.files_name.source = args.value[1];
	tmp = fopen(args.value[1], "r");

	if(tmp == NULL){
		if(args.value[1][0] == '-'){
			ERROR_file_name_expected_instead_flag(args.value[1]);
		}

		ERROR_source_file_not_exist(args.value[1]);
	}

	fclose(tmp);

	if(args.value[1][0] == '-'){
		ERROR_file_name_expected_instead_flag(args.value[1]);
	}

	if(!IS_DOT_LUA( args.value[1], strlen(args.value[1])) ){
		ERROR_invalid_file_extension(args.value[1]);
	}
}

void read_other_arguments(void){
	if(args.index == 1)
		return;


	for(short i = 2; i <= args.index; i++){

		// invalid
		if(flag_cmp(args.value[i], FLAG_VERSION)){
			CHECK_FLAG_REPETITION(flag_cmp(args.value[i], FLAG_VERSION))
		}

		if(flag_cmp(args.value[i], FLAG_HELP)){
			CHECK_FLAG_REPETITION(flag_cmp(args.value[i], FLAG_HELP))
		}

		///// VALID ONE TIME /////

		CaS_CHAR_FLAG(args.files_name.destine, set_destine_file_name, FLAG_DEST_NAME)
		CaS_CHAR_FLAG(args.flags.lib_name,     set_lib_name,          FLAG_LIB_NAME)

		CaS_BOOL_FLAG(args.flags.verbose,     args.flags.verbose,     true,  FLAG_VERBOSE)
		CaS_BOOL_FLAG(args.flags.replace,     args.flags.replace,     true,  FLAG_REPLACE)
		CaS_BOOL_FLAG(!args.flags.header_file,args.flags.header_file, false, FLAG_NO_HEADER)

		if(args.value[i][0] != '-'){
			ERROR_unexpected_file_name(args.value[i]);
		}
		ERROR_invalid_flag(args.value[i]);
	}
}


void does_dest_file_already_exist(void){
	FILE *tmp;
	tmp = fopen(args.files_name.destine, "r");

	if(tmp != NULL){
		fclose(tmp);
		
		if(!args.flags.replace){
			ERROR_dest_file_already_exist(args.files_name.destine);
		}
	}
}
