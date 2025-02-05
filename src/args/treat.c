#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "process-master.h"
#include "treat.h"

void set_destine_file_name(const char *src){
	const unsigned short len = strlen(src);
	char *tmp;


	tmp = malloc(len + sizeof(char));
	strcpy(tmp, src);


	if(IS_DOT_LUA(tmp, len))
		tmp[len - 4] = '\0';

	strcat(tmp, ".lim");

	args.files_name.destine = tmp;
}

void set_lib_name(const char *src){
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


	args.flags.lib_name = refined;
}
