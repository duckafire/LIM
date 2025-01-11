#include <stdbool.h>
#include <string.h>
#include "../../core/lim-global-variables.h"
#include "../print/flags-in-define.h"
#include "../print/help.h"
#include "../print/error.h"
#include "check-and-treat.h"
#include "flag-man.h"
#include "flags.h"

static int argc;
static char **argv;

#define IS_DOT_LUA(str, len) \
	(len > 4                 \
	&& str[len - 4] == '.'   \
	&& str[len - 3] == 'l'   \
	&& str[len - 2] == 'u'   \
	&& str[len - 1] == 'a')

void check_program_arguments(int c, char *v[]){
	argc = c - 1;
	argv = v;

	is_it_information_flag();
	search_and_set_source_file();

	if(read_other_arguments())
		set_destine_file_name( lim.files.source_name );

	does_dest_file_already_exist();
}

static void is_it_information_flag(void){
#define IGNORED(n) \
	if(argc > n)   \
		printf("[LIM] Arguments above %dth were ignored.\n\n", n)
//#enddef

	if(argc == 0){
		no_arguments_to_lim;
	}

	if(flag_cmp(argv[1], FLAG_VERSION)){
		IGNORED(1);

		print_current_version;
	}

	if(flag_cmp(argv[1], FLAG_HELP)){
		if(argc == 1)
			show_help_messages(NULL);

		IGNORED(2);

		// "help" arguments are checked in:
		// /src/interfaces/args/print/help.c
		show_help_messages(argv[2]);
	}

#undef IGNORED
}

static void search_and_set_source_file(void){
	lim.files.source_name = argv[1];
	lim.files.source = fopen(argv[1], "r");

	if(lim.files.source == NULL){
		if(argv[1][0] == '-'){
			ERROR_file_name_expected_instead_flag(argv[1]);
		}

		ERROR_source_file_not_exist(argv[1]);
	}

	if(argv[1][0] == '-'){
		ERROR_file_name_expected_instead_flag(argv[1]);
	}

	if(!IS_DOT_LUA( argv[1], strlen(argv[1])) ){
		ERROR_invalid_file_extension(argv[1]);
	}
}

static bool read_other_arguments(void){
	if(argc == 1)
		return true;


#define REPETITION(cond)                 \
	if(cond){                            \
		ERROR_flag_repetititon(argv[i]); \
	}
//#enddef
#define IS_NULL_NEXT_ARGV \
	i == argc

	for(short i = 2; i <= argc; i++){

		// invalid
		if(flag_cmp(argv[i], FLAG_VERSION)){
			REPETITION(flag_cmp(argv[i], FLAG_VERSION))
		}

		if(flag_cmp(argv[i], FLAG_HELP)){
			REPETITION(flag_cmp(argv[i], FLAG_HELP))
		}


		// valid (one time)
		if(flag_cmp(argv[i], FLAG_DEST_NAME)){
			REPETITION( (lim.files.destine_name != NULL) )

			if(IS_NULL_NEXT_ARGV){
				ERROR_suffix_expected_after_flag(argv[i]);
			}

			set_destine_file_name(argv[++i]);
			continue;
		}

		if(flag_cmp(argv[i], FLAG_UNTIL_STAGE)){
			REPETITION( (lim.flags.until_stage != NULL) )

			// create ONE time, because repetition error
			char *until_state = argv[i];

			if(IS_NULL_NEXT_ARGV){
				ERROR_suffix_expected_after_flag(argv[i]);
			}

			i++;
			if(strlen(argv[i]) > 1 && !(argv[i][0] > '1' && argv[i][0] < '5')){
				ERROR_invalid_suffix_to_flag(until_state, "1-5", argv[i]);
			}

			lim.flags.until_stage = argv[i];
			continue;
		}


#define CMP_AND_SET(cond, var, val, ...) \
	if(flag_cmp(argv[i], __VA_ARGS__)){  \
		REPETITION( (cond) )             \
		var = val;                       \
		continue;                        \
	}

		CMP_AND_SET(lim.flags.verbose,      lim.flags.verbose,     true,  FLAG_VERBOSE)
		CMP_AND_SET(lim.flags.replace,      lim.flags.replace,     true,  FLAG_REPLACE)
		CMP_AND_SET(!lim.flags.header_file, lim.flags.header_file, false, FLAG_NO_HEADER)

		if(argv[i][0] == '-'){
			ERROR_unexpected_file_name(argv[i]);
		}
		ERROR_invalid_flag(argv[i]);
	}


#undef REPETITION
#undef CMP_AND_SET
#undef IS_NULL_NEXT_ARGV

	return (lim.files.destine_name == NULL);
}

static void set_destine_file_name(const char *src){
	const unsigned short len = strlen(src);
	char *tmp;


	tmp = malloc(len + 1);
	strcpy(tmp, src);


	if(IS_DOT_LUA(tmp, len))
		tmp[len - 4] = '\0';

	strcat(tmp, ".lim");

	lim.files.destine_name = tmp;
}

static void does_dest_file_already_exist(void){
	lim.files.destine = fopen(lim.files.destine_name, "r");

	if(lim.files.destine != NULL && !lim.flags.replace){
		ERROR_dest_file_already_exist(lim.files.destine_name);
	}
}
