#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
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
	//return; // TODO: work in progress
	
	// variables and tables declared in library
	// environment, with the keyword `local`;
	// and functions prefixed by `local`.
	bool isLibIdent = false;

	// variables and tables prefixeds by `_G`,
	// declared in some place; and functions
	// not prefixed by `local`
	bool isGlobalIdent = false;

	// always that a code block was finded,
	// the environment "deep" will be increased
	// and a new `LocalEnv` will be created;
	// after that the end of the code block is
	// finded, it will be decreased and the last
	// object will be freed
	unsigned int envLayer = 0;

	FILE *buf;
	char *word;
	ident_init();

	fclose(gf_origin);
	gf_origin = NULL;

	buf = copyFile(collect_get(), NULL);

	char c = 0;
	/*
	while((c = fgetc(buf)) != EOF){
		if(c != '\n'){
			ident_add(c);
			continue;
		}

		word = ident_get();

		if(strcmp(word, "end"))
			//envLayer--;

		if(isalpha(word[0]) != 0 || word[0] == '_'){
			// indetifiers
			//envLayer++;
			//global_newEnv();

			// functions
			if(word[strlen(word) - 1] == '('){
				if(isLibIdent)
					global_print(word, ENV_LIB_FUNC);
				else if(isGlobalIdent)
					global_print(word, ENV_GLOBAL_FUNC);
				else
					global_print(word, ENV_LOCAL_FUNC);

			// metamethods
			}else if(word[1] == '_'){
				global_print(word, ENV_CONSTANTS);

			// library global
			}else if(word[1] == 'G'){
				global_print(word, ENV_LIB_VAR);

			// variables and tables
			}else{
				if(isLibIdent)
					global_print(word, ENV_LIB_VAR);
				else if(isGlobalIdent)
					global_print(word, ENV_GLOBAL_VAR);
				else
					global_print(word, ENV_LOCAL_VAR);
			}

		// special characteres, numbers
		// and table keys
		}else{
			global_print(word, ENV_CONSTANTS);
		}

		ident_end(true);
	}
	*/

	fclose(buf);
}

void cp_x_tempFinish(void){
	//fclose(copyFile(collect_get(), "output.lim"));
	info_verbose();
	collect_end();
	info_verbose();
}
