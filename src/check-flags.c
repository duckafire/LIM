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

	if(t_strcmp2(argv[1], F_VERSION))
		info_version();

	if(t_strcmp2(argv[1], F_HELP)){
		if(argc == 2)
			info_help(NULL);

		info_help(argv[2]);
	}
}

void cf_unexpected(void){
	// they can be used only like 1th argument
	char invalid[INFO_FLAGS][2][LARGEST_FLAG] = {
		{F_VERSION},
		{F_HELP},
	};

	for(short i = 2; i < argc; i++)
		for(short j = 0; j < ARRAY_LEN(invalid[i]); j++)
			if(t_strcmp2(argv[i], invalid[j][0], invalid[j][1]))
				er_unexpectedFlag(argv[i], i);
}

void cf_toCompaction(void){ // "destineName_1"
	for(short i = 1; i < argc; i++){
		if(t_strcmp2(argv[i], F_VERBOSE)){
			if(flags.verbose)
				er_repeatFlag(argv[i], i);

			flags.verbose = true;
			argv[i] = NULL;
			continue;
		}

		if(t_strcmp2(argv[i], F_NAME)){
			if(lim.destineFileName != NULL)
				er_repeatFlag(argv[i], i);

			if(i + 1 == argc)
				er_argExpected( t_getLongFlag(F_NAME) );

			cf_setDestineName(argv[i + 1], true);

			argv[i] = NULL;
			argv[i + 1] = NULL;
			continue;
		}
		
		if(t_strcmp2(argv[i], F_REPLACE)){
			if(flags.replace)
				er_repeatFlag(argv[i], i);

			flags.replace = true;
			argv[i] = NULL;
			continue;
		}

		if(t_strcmp2(argv[i], F_NO_HEAD)){
			if(!flags.headfile)
				er_repeatFlag(argv[i], i);

			flags.headfile = false;
			argv[i] = NULL;
			continue;
		}

		if(t_strcmp2(argv[i], F_UNTIL_S)){
			if(flags.untilStage != 0)
				er_repeatFlag(argv[i], i);

			if(i + 1 == argc)
				er_argExpected( t_getLongFlag(F_UNTIL_S) );

			if(strlen(argv[i + 1]) > 1 || argv[i + 1][0] < '1' || argv[i + 1][0] > '4')
				er_invalidSuffixToFlag( t_getLongFlag(F_UNTIL_S) , "number (1 - 4)", argv[i + 1]);

			flags.untilStage = (short)(argv[i + 1][0] - '0');

			argv[i] = NULL;
			argv[i + 1] = NULL;
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

		if(lim.sourceFileName == NULL){
			lim.sourceFileName = argv[i];
			continue;
		}

		er_filesNamesOverflow;
	}
}

void cf_originName(){
	if(lim.sourceFileName == NULL)
		er_nameNotSpecified;
}

void cf_destineName_2(void){
	if(lim.destineFileName == NULL){
		short len = strlen(lim.sourceFileName);

		char *temp;
		temp = malloc(len);
		memset(temp, '\0', sizeof(temp));
		strcpy(temp, lim.sourceFileName);

		// remove extension (".lua")
		if(lim.sourceFileName[len - 4] == '.'
		&& lim.sourceFileName[len - 3] == 'l'
		&& lim.sourceFileName[len - 2] == 'u'
		&& lim.sourceFileName[len - 1] == 'a')
			temp[len - 4] = '\0';

		cf_setDestineName(temp, false);
	}
}

static void cf_setDestineName(char *src, bool withPath){
	unsigned int len = strlen(src);
	lim.destineFileName = malloc(len + (sizeof(char) * 5));

	if(withPath){
		strcpy(lim.destineFileName, src);
		strcat(lim.destineFileName, ".lim\0");
		return;
	}

	// put in current directory
	unsigned int srcId = 0;

	// ../path/to/origin.lua
	for(unsigned int i = 0; i <= len; i++)
		if(src[i] == '/')
			srcId = i + 1;

	// origin
	for(unsigned int i = 0; i < len + 5; i++){
		lim.destineFileName[i] = src[ srcId ];

		if(src[ srcId ] == '\0')
			break;

		srcId++;
	}

	// origin.lim
	strcat(lim.destineFileName, ".lim\0");
}
