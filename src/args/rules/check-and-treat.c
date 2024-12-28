#include <stdbool.h>
#include <string.h>
#include "../../master/lim-global-variables.h"
#include "../print/flags-in-define.h"
#include "../print/flags-in-function.h"
#include "../print/error.h"
#include "check-and-treat.h"
#include "flag-man.h"
#include "flags.h"

static int argc;
static char **argv;

void check_program_arguments(int c, char *v[]){
	argc = c - 1;
	argv = v;

	is_it_information_flag();
	search_and_set_source_file();

	if(read_other_arguments())
		set_destine_file_name( lim.files.source_name );
}

static void is_it_information_flag(void){
	if(argc == 0){
		no_arguments_to_lim;
	}

	if(flag_cmp(argv[1], FLAG_VERSION)){
		print_current_version;
	}

	if(flag_cmp(argv[1], FLAG_HELP)){
		if(argc == 1)
			show_help_messages(NULL);

		// "help" arguments are checked in:
		// /src/interfaces/args/print/help.c
		show_help_messages(argv[2]);
	}
}

static void search_and_set_source_file(void){
	lim.files.source_name = argv[1];
	lim.files.source = fopen(argv[1], "r");

	if(lim.files.source == NULL){
		ERROR_source_file_not_exist(argv[1]);
	}
}

static bool read_other_arguments(void){
	if(argc == 1)
		return true;


#define REPETITION(cond)                 \
	if(cond){                            \
		ERROR_flag_repetititon(argv[i]); \
	}


	for(short i = 1; i <= argc; i++){

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

			if(i == argc){
				ERROR_suffix_expected_after_flag(argv[i]);
			}

			set_destine_file_name(argv[++i]);
			continue;
		}

		if(flag_cmp(argv[i], FLAG_UNTIL_STAGE)){
			REPETITION( (lim.flags.until_stage != NULL) )

			i++;
			if(strlen(argv[i]) > 1 && !(argv[i][0] > '1' && argv[i][0] < '5')){
				ERROR_unvalid_suffix_to_flag("-us | --until-stage", "1-5", argv[i]);
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
	}


#undef REPETITION
#undef CMP_AND_SET

	return (lim.files.destine_name == NULL);
}

static void set_destine_file_name(const char *src){
	const unsigned short len = strlen(src);
	char *tmp;


	tmp = malloc(len + 1);
	strcpy(tmp, src);


	if(len > 4
	&& tmp[len - 4] == '.'
	&& tmp[len - 3] == 'l'
	&& tmp[len - 2] == 'u'
	&& tmp[len - 1] == 'a')
		tmp[len - 4] = '\0';


	lim.files.destine_name = tmp;
}
