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

	// related the verification of the
	// lua keywords
	short luaKW = LUA_NONE_KW;

	// index to difference the names of
	// different anonymous functions
	unsigned int anonyId = 0;
	
	// this is used to find function end;
	// `0` is equal root environment
	unsigned short codeBlockLayer = 0;

	// base in TYPE constants
	short envCode = TYPE_NONE;

	// used to readl "content" (file)
	char c = 0;

	// dinamic string that store the
	// functions (environment) name;
	// `NULL` indicate that the "cursor"
	// is in root environment
	char *anonyName = NULL;

	// store the function name when the
	// "cursor" is inside a
	char *funcName = NULL;

	// current word
	char *word = NULL;

	// store the content getted by
	// last function called by the
	// "compaction-process"
	FILE *content;

	ident_init();
	global_init();

	fclose(gf_origin);
	gf_origin = NULL;

	content = copyFile(collect_get(), NULL);

	while((c = fgetc(content)) != EOF){
		if(c != '\n'){
			ident_add(c);
			continue;
		}

		envCode = TYPE_NONE;
		word = ident_get();
		luaKW = checkLuaKeywords(word);

		// "check prefixes now"
		if(luaKW == LUA_NONE_KW || luaKW == LUA_NOB)
			prefix = checkPrefixNow(word, prefix);
		
		if(luaKW == LUA_NOB)
			envCode = TYPE_CONSTANT;

		// outside functions
		if(isRootEnv){
			isFunc = (prefix == PREFIX_LIB_FUNC || prefix == PREFIX_GLOBAL_FUNC || prefix == PREFIX_LOCAL_FUNC);

			if(prefix == PREFIX_ANONYMOUS){
				isRootEnv = false;
				isAnony = true;

				free(anonyName);
				anonyName = malloc(strlen(word) + 1 + INT_LEN(anonyId));

				sprintf(anonyName, "%s%u", word, anonyId);
				funcName = anonyName;
			}

			if(isFunc || isAnony){
				global_newEnv(funcName);
				codeBlockLayer++;
			}

		// inside a function
		}else{
			if(luaKW == LUAB_OPEN)
				codeBlockLayer++;
			else if(luaKW == LUAB_CLOSE && codeBlockLayer > 0)
				codeBlockLayer--;

			if(codeBlockLayer == 0){
				isRootEnv = true;
				isAnony = false;

				funcName = NULL;
			}
		}

		// save word content
		if(isAnony)
			envCode = TYPE_CONSTANT;
		else if(envCode != TYPE_CONSTANT)
			envCode = contentType(word, prefix);

		global_order(envCode);
		global_print(word, funcName, envCode);

		// "check prefixes to next cycle"
		if(envCode == TYPE_CONSTANT)
			prefix = checkPrefixNextCycle(word, isRootEnv);

		ident_end(true);
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
