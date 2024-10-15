#include <stdio.h>
#include <string.h>

#include "global.h"
#include "flags.h"
#include "errors.h"

bool g_verbose = false;
bool g_replace = false;

char *gp_nameOrg;
char *gp_nameDst;

int main(int argc, char *argv[]){
	// information (exit)
	if(argc == 1)
		welcome();

	if(strcmp2(argv[1], F_VERSION))
		version();

	if(strcmp2(argv[1], F_H_LIST))
		helpList();

	if(strcmp2(argv[1], F_HELP)){
		if(argc == 2) help(NULL);
		help(argv[2]);
	}

	if(strcmp2(argv[1], F_LICENSE))
		license();

	// check the incorrect use of "information" flags
	char invalid[3][2][LARGEST_FLAG] = {
		{F_VERSION},
		{F_H_LIST},
		{F_HELP}
	};

	for(short i = 2; i < argc; i++)
		for(short j = 0; j < ARRAY_LEN(invalid[i]); j++)
			if(strcmp2(argv[i], invalid[j][0], invalid[j][1]))
				unexpectedFlag(argv[i], i);

	// options to compaction
	for(short i = 1; i < argc; i++){
		if(strcmp2(argv[i], F_VERBOSE)){
			if(g_verbose) repeatFlag(argv[i], i);
			g_verbose = true;
			argv[i] = NULL;
			continue;
		}
		if(strcmp2(argv[i], F_NAME)){
			if(gp_nameDst != NULL) repeatFlag(argv[i], i);
			if(i + 1 == argc) argExpected(F_NAME);
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

	// search invalid flags
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

	if(gp_nameOrg == NULL)
		nameNotSpecified();

	// get compacted file name (without "--name")
	if(gp_nameDst == NULL){
		short maxToFor = strlen(gp_nameOrg) + 1;

		char temp[maxToFor], temp2[maxToFor];
		memset(temp,  '\0', sizeof(temp ));
		memset(temp2, '\0', sizeof(temp2));

		// remove extension (".lua")
		if(gp_nameOrg[maxToFor - 5] == '.'
		&& gp_nameOrg[maxToFor - 4] == 'l'
		&& gp_nameOrg[maxToFor - 3] == 'u'
		&& gp_nameOrg[maxToFor - 2] == 'a')
			for(short i = 0; i < maxToFor - 5; i++)
				temp[i] = gp_nameOrg[i];
		else
			strcpy(temp, gp_nameOrg);

		strcpy(temp2, temp);
		gp_nameDst = temp2;

	}

	//startedProcess();
	printf("Output: %s, %s, %d, %d\n\n", gp_nameOrg, gp_nameDst, g_replace, g_verbose);
	return 0;
}
