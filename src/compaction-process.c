#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "heads.h"

static char c;

void cp_0_checkAndOpenFiles(void){
	info_verbose(VM_NORMAL, "COMPACTION STARTED!");
	info_verbose(VM_TITLE, "STAGE 0: check specified files.");

	// check all files (exit or not)
	info_verbose(VM_NORMAL, "Checking and openning origin file...");
	gf_origin = fopen(gp_nameOrg, "r");
	if(gf_origin == NULL)
		er_nonExistentFile(gp_nameOrg);

	if(g_replace){
		info_verbose(VM_NORMAL, "REPLACE: ON =x");

	}else{
		info_verbose(VM_NORMAL, "REPLACE OFF => checking if destine file already exist...");

		FILE *dst;
		dst = fopen(gp_nameDst, "r");

		if(dst != NULL){
			fclose(dst);
			er_fileAlreadyExistent(gp_nameDst);
		}
	}

	// search and get the "header.lim"
	info_verbose(VM_NORMAL, "Loading \"header.lim\":...");
	info_verbose(VM_NORMAL, head_init());
}

void cp_1_extractionFromOrigin(void){
	info_verbose(VM_TITLE, "STAGE 1: extrat content from origin.");

	// for not add line feed after dot
	// from float number
	bool isFloat = false;

	// to accept all hexadecimal
	// characteres in number check
	bool isHex = false;

	// used to identify string after
	// "normal words"
	bool isString = false;

	// used to protect methods name, like:
	// function tab.met0() end
	// function tab:met1() end
	bool wasFunc = false;

	info_verbose(VM_START_BUF, "collect", "ident", NULL);
	collect_init();
	ident_init();

	info_verbose(VM_START_PRO, "extract", NULL);
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
		if(ct_getIdentifier(&c, true)){
			do{
				// the dot in first will protect
				// this key against the compaction
				// process
				if(c == '.' && !wasFunc){
					collect_add('\n');
					ident_add('\n');
				}

				collect_add(c);
				ident_add(c);

				// this parenthesis specify that
				// this identify belongs to function
				if(c == '(')
					break;

				FGETC;
			}while(ct_getIdentifier(&c, false));

			isString = (c == '\'' || c == '"');
			wasFunc = (strcmp(ident_get(), "function") == 0);
			
			ident_end(true);
			collect_add('\n');

			// without this condition, strings finded
			// after a "word" will be treated incorrectly
			if(isString){
				ct_saveString(c);
				continue;
			}

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
					c = ct_clearSpaces();

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
		ct_getSpecial(c);
	}
	info_verbose(VM_END_PRO, "extract", NULL);

	info_verbose(VM_END_BUF, "ident", NULL);
	ident_end(false);

	info_verbose(VM_NORMAL, "Closing origin file.");
	fclose(gf_origin);
	gf_origin = NULL;
}

void cp_2_separateExtractedContent(void){
	info_verbose(VM_TITLE, "STAGE 2: separate extracted content.");

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
	unsigned int anonyId = 0;
	
	// this is used to find function end;
	// `0` is equal root environment
	unsigned short codeBlockLayer = 0;

	// base in TYPE constants
	short typeCode = TYPE_NONE;

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

	info_verbose(VM_START_BUF, "ident", "global", NULL);
	ident_init();
	global_init();

	info_verbose(VM_NORMAL, "Getting extracted content...");
	content = tools_copyFile(collect_get(), NULL);

	info_verbose(VM_END_BUF, "collect", NULL);
	collect_end();

	info_verbose(VM_START_PRO, "separate", NULL);
	while((c = fgetc(content)) != EOF){
		if(c != '\n'){
			ident_add(c);
			continue;
		}

		word = ident_get();

		// get and set content (word) type
		typeCode = TYPE_NONE;
		
		if(prefix != PREFIX_NONE)
			typeCode = readPrefix(word, prefix, isRootEnv);

		if(typeCode == TYPE_NONE)
			typeCode = readCurWord(word);

		prefix = setPrefix(word, prefix, isRootEnv);

		// outside functions
		if(isRootEnv){
			funcName = checkAndCreateNewEnv(word, typeCode);

			if(funcName != NULL){
				global_newEnv(funcName);
				codeBlockLayer++;
				isRootEnv = false;
			}
		// inside a function
		}else{
			checkAndUpLayer(word, &codeBlockLayer);

			if(codeBlockLayer == 0){
				isRootEnv = true;
				isAnony = false;

				funcName = NULL;
			}
		}

		global_order(typeCode);
		global_print(word, funcName, typeCode);

		ident_end(true);
	}
	info_verbose(VM_END_PRO, "separate", NULL);

	info_verbose(VM_END_BUF, "ident", NULL);
	free(anonyName);
	ident_end(false);
	fclose(content);
}

void cp_3_buildingGlobalScope(void){
	// REFERENCE SCOPE
	info_verbose(VM_TITLE, "STAGE 3: building global scope.");

	// length of "word"; used to get its lat
	// character and/or set a dinamic pointer size
	unsigned short len;

	// math, string, table, ...; if it suffix will
	// not be valid, it will be treaty like "word"
	char *table = NULL;

	// store the current word collected; if it do
	// not have dot or parenthesis, it is a "table",
	// otherwise it is a function (pairs, type, ...);
	// different the before stage, here the "word"
	// not store the "word_get()" pointer, it store
	// a copy of the content from this "object", in
	// a independent pointer.
	char *word = NULL;

	// lists of functions that will be to win a
	// reference (from Lua and from "header.lim")
	FILE *fileList[2] = {
		tools_copyFile((global_get())->luaFunc, NULL),
		tools_copyFile((global_get())->headFunc, NULL),
	};

	info_verbose(VM_START_BUF, "ident", "refe (tree)", NULL);
	ident_init();
	refe_init();

	info_verbose(VM_START_PRO, "collect", NULL);
	for(short fileId = 0; fileId < 2; fileId++){
		if(fileList[ fileId ] == NULL)
			break;

		fseek(fileList[ fileId ], 0, SEEK_SET);

		while((c = fgetc(fileList[ fileId ])) != EOF){
			if(c != '\n'){
				ident_add(c);
				continue;
			}
			
			if(word != NULL)
				free(word);

			len = strlen(ident_get());
			word = malloc(len + 1);
			strcpy(word, ident_get());
			ident_end(true);

			// word is a table
			if(word[0] != '.' && word[len - 1] != '(' && (fileId == 1 || checkLuaTabs(word))){
				// a table was collected, but after it,
				// none function was placed
				if(table != NULL){
					refe_add(NULL, table);
					free(table);
				}

				// copy word content to print
				// it the next cycle
				table = malloc(len + 1);
				strcpy(table, word);
				continue;
			}

			// print the table finded before with
			// the function finded right now
			if(table != NULL){
				refe_add(table, word);
				free(table);
				free(word);
				table = NULL;
				word = NULL;
				continue;
			}

			// function no prefixed by a table
			// (tonumber, next, select, ...)
			refe_add(NULL, word);
			free(word);
			word = NULL;
		}

		if(word != NULL){
			refe_add(NULL, word);
			free(word);
			word = NULL;
		}

		if(table != NULL){
			refe_add(NULL, table);
			free(table);
			table = NULL;
		}
	}
	info_verbose(VM_END_PRO, "collect", NULL);

	info_verbose(VM_END_BUF, "ident", NULL);
	ident_end(false);

	info_verbose(VM_START_BUF, "refe (queue)", NULL);
	refe_treeToQueue();

	info_verbose(VM_END_BUF, "refe (tree)", NULL);
	refe_endTree();

	info_verbose(VM_START_BUF, "scope", "nick", "pair", NULL);
	scope_init();
	nick_init(true);
	pair_init();

	char pairId = '\0';
	char *fullContent;
	char *fullCttBuf;
	RefeQueue *item;

	info_verbose(VM_NORMAL, "Adding \"local\" keyword to functions reference scope.");
	scope_add("local ", SCOPE_FUNC);
	scope_rmvLastComma(SCOPE_FUNC);

	// all references are readed and classified, now they need
	// to be positioned in a "Lua declaration structure":
	// local ident0,ident1,identn=value0,value1,valuen
	info_verbose(VM_START_PRO, "build", NULL);
	while((item = refe_getAndRmvQueueItem()) != NULL){
		pairId = '\0';

		// get full content from queue item
		fullContent = malloc(sizeof(char) * 2);
		strcpy(fullContent, "\0\0");

		// get
		if(item->origin != NULL){
			fullCttBuf = malloc(strlen(item->origin) + 1);
			strcpy(fullCttBuf, item->origin);

			free(fullContent);
			fullContent = fullCttBuf;

			pairId = item->content[1];
		}else{
			pairId = item->content[0];
		}

		// get (and merge)
		fullCttBuf = malloc(strlen(fullContent) + strlen(item->content) + 1);
		strcpy(fullCttBuf, fullContent);
		strcat(fullCttBuf, item->content);

		free(fullContent);
		fullContent = fullCttBuf;

		// print content getted from
		// item in specify buffers
		scope_add(nick_get(), SCOPE_FUNC);
		scope_add(fullContent, SCOPE_ADDR);

		pair_add(pairId, nick_get(), fullContent);

		// free dinamic memories
		if(item->origin != NULL)
			free(item->origin);

		if(item->content != NULL);
			free(item->content);

		free(item);
		free(fullContent);
		nick_up();
	}
	info_verbose(VM_END_PRO, "build", NULL);

	// merge functions values (addre)
	// with their references (func)
	info_verbose(VM_NORMAL, "Merging the reference with their respective functions...");

	if(tools_filelen(scope_get(SCOPE_ADDR)) > 0){
		// summaries
		FILE *src, *dest;
		src  = scope_get(SCOPE_ADDR);
		dest = scope_get(SCOPE_FUNC);

		fseek(src, 0, SEEK_SET);
		scope_rmvLastComma(SCOPE_FUNC); // "it" == dest
		fputc('=', dest);

		// "identifiers" and "address" were positioned in
		// different buffers:
		// [BUF 0] local ident0,ident1,identn
		// [BUF 1] value0,value1,valuen
		// it loop will merge them:
		// local identn=valuen
		tools_fcat(src, dest);

		scope_rmvLastComma(SCOPE_FUNC); // "it" == dest
		fputc('\n', dest);
		info_verbose(VM_NORMAL, "Merge finished.");
	}else{
		info_verbose(VM_NORMAL, "None function found for scope: empty reference scope.");
	}

	// actually, "refe (queue)" is
	// ended during the build process
	info_verbose(VM_END_BUF, "refe (queue)", "scope", "nick", NULL);
	nick_end();



	// VARIABLES SCOPE
	FILE *variables;
	variables = tools_copyFile((global_get())->var, NULL);

	info_verbose(VM_START_BUF, "ident", "nick", NULL);
	ident_init();
	nick_init(false);

	if(tools_filelen(scope_get(SCOPE_VAR)) > 0){
		info_verbose(VM_NORMAL, "Adding \"local\" keyword to variables and tables scope.");
		scope_add("local ", SCOPE_VAR);
		scope_rmvLastComma(SCOPE_VAR);
	}

	info_verbose(VM_START_PRO, "collect (2)", NULL);
	while((c = fgetc(variables)) != EOF){
		if(c != '\n'){
			ident_add(c);
			continue;
		}

		pair_add(ident_get()[0], nick_get(), ident_get());

		scope_add(ident_get(), SCOPE_VAR);
		nick_up();
		ident_end(true);
	}
	info_verbose(VM_END_PRO, "collect (2)", NULL);

	if(tools_filelen(scope_get(SCOPE_VAR)) > 0){
		scope_rmvLastComma(SCOPE_VAR);
	}else{
		info_verbose(VM_NORMAL, "None variable found for scope: empty variable scope.");
	}
	
	info_verbose(VM_END_BUF, "ident", NULL);
	ident_end(false);
}

void cp_4_organizeAndCompact(void){
	info_verbose(VM_TITLE, "STAGE 4: organize and compact identifiers");

	// the word getted from "curBuf"
	char *word = NULL;

	// index used to navigate in "pair tree"
	char wordId;

	// sub-buffer, from buffer global, obtained
	// by index stored in sub-buffer global.order
	FILE *curBuf;

	// name of "function environments", that store
	// itself local variables, tables and functions;
	// NULL is equal "root environment"
	char *bufName = NULL;

	// index getted from global.order
	short bufId;

	// index of the current character, from "word",
	// that will be printted in buffer finalContent
	// (print "char. by char.")
	unsigned short wId;

	// current character, obtained from the index
	// "wId", from "word" (print "char. by char.")
	char *wChar;

	// "types" that start a function environment,
	// in root environment
	short funcTypes[4] = {
		TYPE_ANONYMOUS,
		TYPE_LIB_FUNC,
		TYPE_GLOBAL_FUNC,
		TYPE_LOCAL_FUNC,
	};

	// this is used to find function end;
	// `0` is equal root environment
	unsigned short codeBlockLayer = 0;

	// specify when put a space character:
	// if the last character of the current
	// word is a alpha numeric and the first
	// character of the next word also is.
	bool space = false;

	// during the STAGE 1, tables and their
	// keys are separated, but in STAGE 3,
	// they are merged, because of this, to
	// find them in buffer pair, it is
	// necessary that the "word" store the
	// table and it key
	char *fullContent = NULL;

	// temporarily store the pointer of the
	// "fullContent", for free it after that
	// the its address, stored in "fullContent",
	// to be changed
	char *tempFullCtt;

	// used to open parenthesis after write a
	// function, because them lose their first
	// parenthesis during compaction process
	bool isFunc = false;

	global_fseekSetAll();

	info_verbose(VM_START_BUF, "ident", "final", NULL);
	ident_init();
	final_init();

	info_verbose(VM_START_PRO, "compact");
	while(global_getOrder(&bufId)){
		wId = 0;
		wChar = '\0';
		word = NULL;
		bufName = NULL;
		isFunc = false;
		curBuf = global_getBuf(bufId, bufName);

		while((c = fgetc(curBuf)) != '\n' && c != EOF)
			ident_add(c);

		// check current environment
		if(codeBlockLayer == 0)
			for(short i = 0; i < 4; i++)
				if(bufId == funcTypes[i]){
					bufName = tools_rmvParen(word);
					codeBlockLayer++;
					break;
				}

		word = ident_get();

		if(fullContent != NULL){
			tempFullCtt = fullContent;

			fullContent = malloc(strlen(tempFullCtt) + 1);
			strcpy(fullContent, tempFullCtt);
			strcat(fullContent, word);

			// function from table: math .[ r ]andom
			wordId = word[1];

			// "ident" will be freed in cycle end
			free(tempFullCtt);

			// "word" will be not used in "free()",
			// but "fullContent" will be, then the
			// address of both will be freed
			word = fullContent;

		}else if(checkLuaTabs(word)){ // && `fullContent == NULL`
			fullContent = malloc(strlen(word) + 1);
			strcpy(fullContent, word);

			ident_end(true);
			continue;
		}

		// get word obtained from "curBuf"
		if(bufName == NULL && bufId != TYPE_CONSTANT && bufId != TYPE_ANONYMOUS){
			if(word[ strlen(word) - 1 ] == '(')
				isFunc = true;
			
			// "free" function: [ s ]etmetatable
			if(wordId == '\0')
				wordId = word[0];

			word = pair_cmpAndGet(wordId, word); // nickName or normal identifier
		}

		// if it will be "reseted" before, all
		// word will be consider "free functions"
		wordId = '\0';

		// update or lower environment layers
		if(codeBlockLayer > 0)
			checkAndUpLayer(word, &codeBlockLayer);

		// print in buffer (finalContent)
		if(space && (isalnum(word[0]) || word[0] == '_'))
			final_add(' ');

		for(wChar = &word[ wId ]; *wChar != '\0'; wChar = &word[ ++wId ])
			final_add(*wChar);

		if(isFunc)
			final_add('(');

		wId--;
		space = false;
		if(isalnum(word[wId]) || word[wId] == '_')
			space = true;

		ident_end(true);
		if(bufName != NULL)
			free(bufName);
		if(fullContent != NULL){
			free(fullContent);
			fullContent = NULL;
		}
	}
	info_verbose(VM_END_PRO, "compact");

	info_verbose(VM_START_BUF, "ident", "pair", NULL);
	ident_end(false);
	pair_end();
}

void cp_5_mergingContentAndPackingLibrary(void){
	info_verbose(VM_TITLE, "STAGE 5 (final): merge all content and pack library");
	
	// getting content from buffers
	info_verbose(VM_NORMAL, "Getting content from buffers...");
	FILE *fscope, *vscope, *output;
	fscope = scope_get(SCOPE_FUNC);
	vscope = scope_get(SCOPE_VAR);

	fseek(fscope, 0, SEEK_SET);
	fseek(vscope, 0, SEEK_SET);

	// writting in output file
	info_verbose(VM_NORMAL, "Creating output file...");
	output = fopen(gp_nameDst, "w");

	if(head_printTop(output))
		info_verbose(VM_NORMAL, "The \"Top Partition\" was printed.");

	info_verbose(VM_NORMAL, "Starting pack...");
	fputs("local L={}\ndo ", output);

	if(tools_filelen(vscope) > 0){
		info_verbose(VM_NORMAL, "Printing variables and table scope...");
		tools_fcat(vscope, output);
	}else{
		info_verbose(VM_NORMAL, "Empty variables scope");
	}

	if(tools_filelen(fscope) > 7){ // `local ,` length
		info_verbose(VM_NORMAL, "Printing functions references scope...");
		tools_fcat(fscope, output);
	}else{
		info_verbose(VM_NORMAL, "Empty function scope.");
	}

	if(head_printScope(output))
		info_verbose(VM_NORMAL, "The \"Scope Partition\" was printed.");

	info_verbose(VM_NORMAL, "Printing compacted content...");
	tools_fcat(final_get(), output);

	info_verbose(VM_NORMAL, "Closing pack...");
	fputs("end\n--local reference=L", output);

	// finishing
	info_verbose(VM_NORMAL, "Finishing output file...");
	fclose(output);

	info_verbose(VM_END_BUF, "global", "scope", "head", "final", NULL);
	global_end();
	scope_end();
	head_end();
	final_end();

	info_verbose(VM_TITLE, "(alpha) PROCESS FINISHED!");
	info_verbose(VM_NORMAL, "See: \"./output.lim\"");
}
