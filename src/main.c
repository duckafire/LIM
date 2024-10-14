#include <stdio.h>
#include <string.h>

#include "main.h"
#include "flags.h"
#include "errors.h"

bool g_verbose = false;
bool g_replace = false;

char *gp_nameOrg = NULL;
char *gp_nameDst = NULL;

static short strcmp2(char *str, char *v0, char *v1){
	return (strcmp(str, v0) == 0 || strcmp(str, v1) == 0);
}

int main(int argc, char *argv[]){
	argc--; // except argument "lim"

	const short argvlen = ARRAY_LEN(&argv);

	// information (exit)
	if(argc == 0)
		welcome();

	if(strcmp2(argv[0], VERSION))
		version();

	if(strcmp2(argv[0], MANUAL))
		manual();

	if(strcmp2(argv[0], HELP)){
		if(argc == 1) help(NULL);
		help(argv[1]);
	}

	// check the incorrect use of "information" flags
	char invalid[3][2][BIG_FLAG] = {
		{VERSION},
		{MANUAL},
		{HELP}
	};

	for(short i = 1; i < ARRAY_LEN(invalid); i++)
		for(short j = 0; j < ARRAY_LEN(invalid[i]); j++)
			if(strcmp2(argv[i], invalid[j][0], invalid[j][1]))
				unexpectedFlag(i, argv[i]);

	// options to compaction
	for(short i = 0; i < argvlen; i++){
		if(strcmp2(argv[i], VERBOSE)){
			if(g_verbose) repeatFlag(i, argv[i]);
			g_verbose = true;
			argv[i] = NULL;
			continue;
		}
		if(strcmp2(argv[i], NAME)){
			if(gp_nameDst != NULL) repeatFlag(i, argv[i]);
			if(i + 1 == argvlen) argExpected(NAME);
			gp_nameDst = argv[i + 1];
			argv[i] = NULL;
			continue;
		}
		if(strcmp2(argv[i], REPLACE)){
			if(g_replace) repeatFlag(i, argv[i]);
			g_replace = true;
			argv[i] = NULL;
			continue;
		}
	}

	// get files name
	for(short i = 0; i < argvlen; i++){
		if(argv[i][0] == '-' || (argv[i][0] == '-' && argv[i][1] == '-'))
			invalidFlag(argv[i]);

		if(argv[i] == NULL)
			continue;

		if(gp_nameOrg == NULL){
			gp_nameOrg = argv[i];
			continue;
		}

		if(gp_nameDst == NULL){
			gp_nameDst = argv[i];
			continue;
		}

		filesNamesOverflow();
	}

	//startedProcess();
	return 0;
}
