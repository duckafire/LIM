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
	// information (call and exit)
	if(argc == 1)
		info_welcome();

	if(tools_strcmp2(argv[1], F_VERSION))
		info_version();

	if(tools_strcmp2(argv[1], F_H_LIST))
		info_helpList();

	if(tools_strcmp2(argv[1], F_HELP)){
		if(argc == 2)
			info_help(NULL);

		info_help(argv[2]);
	}

	if(tools_strcmp2(argv[1], F_LICENSE))
		info_license();

	if(tools_strcmp2(argv[1], F_RULES))
		info_rules();
}

void cf_unexpected(void){
	// they can be used only like 1th argument
	char invalid[INFO_FLAGS][2][LARGEST_FLAG] = {
		{F_VERSION},
		{F_H_LIST},
		{F_HELP},
		{F_LICENSE},
		{F_RULES},
	};

	for(short i = 2; i < argc; i++)
		for(short j = 0; j < ARRAY_LEN(invalid[i]); j++)
			if(tools_strcmp2(argv[i], invalid[j][0], invalid[j][1]))
				er_unexpectedFlag(argv[i], i);
}

void cf_toCompaction(void){ // "destineName_1"
	// action flags
	
	for(short i = 1; i < argc; i++){
		if(tools_strcmp2(argv[i], F_VERBOSE)){
			if(g_verbose)
				er_repeatFlag(argv[i], i);

			g_verbose = true;
			argv[i] = NULL;
			continue;
		}

		if(tools_strcmp2(argv[i], F_NAME)){
			if(i + 1 == argc)
				er_argExpected(F_NAME);

			if(gp_nameDst != NULL)
				er_repeatFlag(argv[i], i);

			gp_nameDst = argv[i + 1];

			argv[i] = NULL;
			argv[i + 1] = NULL;
			continue;
		}
		
		if(tools_strcmp2(argv[i], F_REPLACE)){
			if(g_replace)
				er_repeatFlag(argv[i], i);

			g_replace = true;
			argv[i] = NULL;
			continue;
		}

		if(tools_strcmp2(argv[i], F_NO_HEAD)){
			if(!g_headfile)
				er_repeatFlag(argv[i], i);

			g_headfile = false;
			argv[i] = NULL;
			continue;
		}
	}
}

void cf_invalid(void){
	// and get name to origin file

	for(short i = 1; i < argc; i++){
		// it receive NULL from
		// "cf_toCompaction"
		if(argv[i] == NULL)
			continue;

		if(argv[i][0] == '-' || (argv[i][0] == '-' && argv[i][1] == '-'))
			er_invalidFlag(argv[i], i);

		if(gp_nameOrg == NULL){
			gp_nameOrg = argv[i];
			continue;
		}

		er_filesNamesOverflow();
	}
}

void cf_originName(){
	if(gp_nameOrg == NULL)
		er_nameNotSpecified();
}

void cf_destineName_2(void){
	if(gp_nameDst == NULL){
		short len = strlen(gp_nameOrg);

		char *temp;
		temp = malloc(len);
		memset(temp, '\0', sizeof(temp));
		strcpy(temp, gp_nameOrg);

		// remove extension (".lua")
		if(gp_nameOrg[len - 4] == '.'
		&& gp_nameOrg[len - 3] == 'l'
		&& gp_nameOrg[len - 2] == 'u'
		&& gp_nameOrg[len - 1] == 'a')
			temp[len - 4] = '\0';

		gp_nameDst = temp;
	}
}
