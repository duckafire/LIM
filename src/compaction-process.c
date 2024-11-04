#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "heads.h"

static char c;

void cp_0_checkAndOpenFiles(void){
	info_verbose();

	gf_origin = fopen(gp_nameOrg, "r");
	if(gf_origin == NULL)
		nonExistentFile(gp_nameOrg);

	if(!g_verbose){
		FILE *dst;
		dst = fopen(gp_nameDst, "r");

		if(dst != NULL){
			fclose(dst);
			fileAlreadyExistent(gp_nameDst);
		}
	}

	collect_init();
}

void cp_1_extractionFromOrigin(void){
	info_verbose();

	// for not add line feed after dot
	// from float number
	bool isFloat = false;

	// to accept all hexadecimal
	// characteres in number check
	bool isHex = false;

	while(FGETC != EOF){
		// clear "null caracteres", without
		// `continue` cycle: They are:
		// // spaces
		// // tabulations
		// // line feed
		// // binaries
		if(!isgraph(c))
			while(!isgraph(FGETC) && c != EOF);

		// get identifiers to:
		// // variables
		// // tables
		// // tables keys
		// // functions
		if(getIdentifier(&c, true)){
			do{
				// the dot in first will protect
				// this key against the compaction
				// process
				if(c == '.')
					collect_add('\n');

				collect_add(c);

				// this parenthesis specify that
				// this identify belongs to function
				if(c == '(')
					break;

				FGETC;
			}while(getIdentifier(&c, false));

			collect_add('\n');
			
			// if the characterer that stop the loop
			// is not pritend here, it will be lost
			if(c != '(' && !isalnum(c) && isgraph(c)){
				collect_add(c);
				collect_add('\n');
			}
			continue;
		}

		// constant numbers:
		// integer
		// float
		if(isdigit(c)){
			do{
				collect_add(c);
				FGETC;

				if(c == ' ')
					c = clearSpaces();

				// when the first dot is finded, `isFloat`
				// becomes `true`, after the loop cycle
				// come back to here it will become `false`,
				// and it cannot become `true` again in this
				// loop
				if(!isHex && (c == '.' || isFloat))
					isFloat = !isFloat;

				// same energ
				else if(c == 'x' && !isFloat)
					isHex = !isHex;

			}while(isdigit(c) || isFloat || (isHex && (isxdigit(c) || c == 'x')));

			isFloat = isHex = false;
			collect_add('\n');
			fseek(gf_origin, -1, SEEK_CUR);
			continue;
		}

		// special characteres (:,.{-+)
		getSpecial(c);
	}
}

void cp_2_separateExtractedContent(void){
	// root of the library environment
	// (outside functions)
	bool isRootEnv = true;

	// set that a anonymous function was
	// finded: `function() end`
	bool isAnony = false;

	// summary of a big condition
	bool isFunc = false;

	// used for guide in decision
	short prefix = PREFIX_NONE;

	// index to difference the names of
	// different anonymous functions
	double anonyId = 0.0;

	// base in TYPE and used to set ENV
	short envCode;

	// environment name; NULL if it is
	// the root environment
	char *envName = NULL;

	// dinamic string that store the
	// functions (environment) name
	char *anonyName = NULL;

	// current word
	char *word = NULL;

	// store the content getted by
	// last function called by the
	// "compaction-process"
	FILE *content;

	ident_init();
	fclose(gf_origin);
	gf_origin = NULL;

	content = copyFile(collect_get(), NULL);

	char c = 0;
	global_init();

	while((c = fgetc(content)) != EOF){
		if(c != '\n'){
			ident_add(c);
			continue;
		}

		// save in a buffer
		word = ident_get();
		envName = NULL;

		// "check prefixes now"
		if(isRootEnv){
			prefix = checkPrefixNow(word, prefix);

			isFunc = (prefix == PREFIX_LIB_FUNC || prefix == PREFIX_GLOBAL_FUNC || prefix == PREFIX_LOCAL_FUNC);

			if(isFunc && isAnony){
				isAnony = false;
				free(anonyName);

			}else if(prefix == PREFIX_ANONYMOUS){
				isAnony = true;

				anonyName = malloc(sizeof(word) + INT_LEN(anonyId++));

				strcpy(anonyName, word);
			}
		}

		if(isAnony)
			envCode = TYPE_CONSTANT;
		else
			envCode = contentType(word, prefix);

		if(isAnony)
			envName = anonyName;
		else
			envName = word;

		global_print(word, envName, envCode);
		ident_end(true);

		// "check prefixes to next cycle"
		if(envCode == TYPE_CONSTANT)
			prefix = checkPrefixNextCycle(word, isRootEnv);
	}

	if(isAnony)
		free(anonyName);

	fclose(content);
}

void cp_x_tempFinish(void){
	//fclose(copyFile(collect_get(), "output.lim"));
	info_verbose();
	collect_end();
	info_verbose();
}
