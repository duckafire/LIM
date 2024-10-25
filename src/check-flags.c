#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "heads.h"

static int argc;
static char **argv;

void cf_setArgValues(int c, char *v[]){
	argc = c;
	argv = v;
}

void cf_single(void){
	// information (exit)
	if(argc == 1)
		info_welcome();

	if(strcmp2(argv[1], F_VERSION))
		info_version();

	if(strcmp2(argv[1], F_H_LIST))
		info_helpList();

	if(strcmp2(argv[1], F_HELP)){
		if(argc == 2) info_help(NULL);
		info_help(argv[2]);
	}

	if(strcmp2(argv[1], F_LICENSE))
		info_license();

	if(strcmp2(argv[1], F_RULES))
		info_rules();
}

void cf_unexpected(void){
	char invalid[4][2][LARGEST_FLAG] = {
		{F_VERSION},
		{F_H_LIST},
		{F_HELP},
		{F_RULES},
	};

	for(short i = 2; i < argc; i++)
		for(short j = 0; j < ARRAY_LEN(invalid[i]); j++)
			if(strcmp2(argv[i], invalid[j][0], invalid[j][1]))
				unexpectedFlag(argv[i], i);
}

void cf_toCompaction(void){ // "destineName_1"
	for(short i = 1; i < argc; i++){
		if(strcmp2(argv[i], F_VERBOSE)){
			if(g_verbose) repeatFlag(argv[i], i);
			g_verbose = true;
			argv[i] = NULL;
			continue;
		}
		if(strcmp2(argv[i], F_NAME)){
			if(i + 1 == argc) argExpected(F_NAME);

			if(gp_nameDst != NULL) repeatFlag(argv[i], i);
			gp_nameDst = argv[i + 1];

			argv[i] = NULL;
			argv[i + 1] = NULL;
			continue;
		}
		if(strcmp2(argv[i], F_REPLACE)){
			if(g_replace) repeatFlag(argv[i], i);
			g_replace = true;
			argv[i] = NULL;
			continue;
		}
	}
}

void cf_invalid(void){
	for(short i = 1; i < argc; i++){
		if(argv[i] == NULL)
			continue;

		if(argv[i][0] == '-' || (argv[i][0] == '-' && argv[i][1] == '-'))
			invalidFlag(argv[i], i);

		if(gp_nameOrg == NULL){ // get origin name
			gp_nameOrg = argv[i];
			continue;
		}

		filesNamesOverflow();
	}
}

void cf_originName(){
	if(gp_nameOrg == NULL)
		nameNotSpecified();
}

void cf_destineName_2(bool *dstUsingMalloc){
	if(gp_nameDst == NULL){
		short maxToFor = strlen(gp_nameOrg) + 1;

		char *temp;
		temp = malloc(maxToFor);
		memset(temp, '\0', sizeof(temp));

		// remove extension (".lua")
		if(gp_nameOrg[maxToFor - 5] == '.'
		&& gp_nameOrg[maxToFor - 4] == 'l'
		&& gp_nameOrg[maxToFor - 3] == 'u'
		&& gp_nameOrg[maxToFor - 2] == 'a')
			for(short i = 0; i < maxToFor - 5; i++)
				temp[i] = gp_nameOrg[i];
		else
			strcpy(temp, gp_nameOrg);

		gp_nameDst = temp;
		*dstUsingMalloc = true;
	}
}
