#include <string.h>
#include <ctype.h>
#include "../core/tools/lim-global-variables.h"
#include "print/flags-in-define.h"
#include "print/error.h"
#include "flags/cmp.h"
#include "flags/list.h"
#include "check-and-treat.h"
#include "help.h"

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

	read_other_arguments();

	if(lim.files.destine_name == NULL)
		set_destine_file_name( lim.files.source_name );

	if(lim.flags.lib_name == NULL)
		set_lib_name( lim.files.source_name );

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

static void read_other_arguments(void){
	if(argc == 1)
		return;


	#define IS_NULL_NEXT_ARGV (i == argc)
	#define REPETITION(cond)                 \
		if(cond){                            \
			ERROR_flag_repetititon(argv[i]); \
		}

	for(short i = 2; i <= argc; i++){

		// invalid
		if(flag_cmp(argv[i], FLAG_VERSION)){
			REPETITION(flag_cmp(argv[i], FLAG_VERSION))
		}

		if(flag_cmp(argv[i], FLAG_HELP)){
			REPETITION(flag_cmp(argv[i], FLAG_HELP))
		}

		///// VALID ONE TIME /////

		#define CMP_AND_CPY_STR(dest, set, ...)                \
			if(flag_cmp(argv[i], __VA_ARGS__)){                \
				REPETITION( (dest != NULL) )                   \
				if(IS_NULL_NEXT_ARGV){                         \
					ERROR_suffix_expected_after_flag(argv[i]); \
				}                                              \
				set(argv[++i]);                                \
				continue;                                      \
			}

		CMP_AND_CPY_STR(lim.files.destine_name, set_destine_file_name, FLAG_DEST_NAME)
		CMP_AND_CPY_STR(lim.flags.lib_name,     set_lib_name,          FLAG_LIB_NAME)

		#define CMP_AND_SET(cond, var, val, ...) \
			if(flag_cmp(argv[i], __VA_ARGS__)){  \
				REPETITION( (cond) )             \
				var = val;                       \
				continue;                        \
			}

		CMP_AND_SET(lim.flags.verbose,      lim.flags.verbose,     true,  FLAG_VERBOSE)
		CMP_AND_SET(lim.flags.replace,      lim.flags.replace,     true,  FLAG_REPLACE)
		CMP_AND_SET(!lim.flags.header_file, lim.flags.header_file, false, FLAG_NO_HEADER)

		if(argv[i][0] != '-'){
			ERROR_unexpected_file_name(argv[i]);
		}
		ERROR_invalid_flag(argv[i]);
	}


	#undef IS_NULL_NEXT_ARGV
	#undef REPETITION
	#undef CMP_AND_SET
	#undef CMP_AND_CPY_STR
}

static void set_destine_file_name(const char *src){
	const unsigned short len = strlen(src);
	char *tmp;


	tmp = malloc(len + sizeof(char));
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

static void set_lib_name(const char *src){
	const unsigned short src_len = strlen(src);
	unsigned short i, brute_len, refined_len;
	short bar_break = -1;
	char *brute, *refined;

	// path/to/fil$.lua
	brute = malloc(src_len + sizeof(char));
	strcpy(brute, src);

	// path/to/fil$
	if(IS_DOT_LUA(brute, src_len))
		brute[src_len - 4] = '\0';

	// path/to/fil$ -> bar_break = 7 (x2 bar)
	for(i = 0; brute[i] != '\0'; i++)
		if(brute[i] == '/')
			bar_break = i;

	// one (or more) bar was found
	if(bar_break != -1){
		brute_len = 0;

		// length file name no path
		for(i = bar_break + 1; brute[i] != '\0'; i++)
			brute_len++;

		refined = malloc(brute_len + sizeof(char));

		// path/to/fil$ -> fil$
		for(i = 0; brute[i] != '\0'; i++)
			refined[i] = brute[bar_break + i + 1];

		free(brute);

	}else{
		refined = brute;
	}


	refined_len = strlen(refined);

	// path/.lua -> path/ ->   -> _
	if(refined_len == 0){
		free(refined);
		refined = malloc(sizeof(char) * 2);
		strcpy(refined, "_");
	
	// fil$ -> fil_
	}else{
		if(refined[0] != '_' && !isalpha(refined[0]))
			refined[0] = '_';

		for(i = 1; i < refined_len; i++)
			if(refined[i] != '_' && !isalnum(refined[i]))
				refined[i] = '_';
	}


	lim.flags.lib_name = refined;
}
