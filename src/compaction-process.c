#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "heads.h"

static char c;           // stage: 1-5
static FILE *extrCttBuf; // stage: 1, 2
static char *string;     // stage 2-5
static FILE *finalCtt;   // stage 4>
static FILE *toexport;   // stage: 2-3

// TODO: add verbose messages

// STRUCTURE:
// declarations
// start "tools"
// loop
// close "tools"

// open source file; and check destine file,
// but only if the flag "--replace" is used
void cp_0_checkAndOpenFiles(void){
	// SOURCE (script specified)
	lim.sourceFile = fopen(lim.sourceFileName, "r");

	if(lim.sourceFile == NULL)
		er_nonExistentFile(lim.sourceFileName);

	// OUTPUT FILE (library)
	if(!flags.replace){
		FILE *dst;
		dst = fopen(lim.destineFileName, "r");

		if(dst != NULL){
			fclose(dst);
			er_fileAlreadyExistent(lim.destineFileName);
		}
	}
}

// get all content from source file; discarte
// spaces, tabulations, comments and line feed;
// ordered in list format:
// local
// message
// =
// "Hello world!"
bool cp_1_extractionFromOrigin(void){
	// for that a line feed will not be added
	// after that a dot is found after a number;
	// only one dot is valid, other are treaty
	// line special characters
	bool isFloat = false;

	// for enable the hexadecimal support after
	// found a 'x', after a '0'; if `true`, it
	// block `isFloat`
	bool isHex = false;

	extrCttBuf = tmpfile();

	while((c = fgetc(lim.sourceFile)) != EOF){
		// clear "null caracteres", without
		// `continue` cycle: They are:
		// space; tabulation; line feed; binary
		if(!isgraph(c))
			while(!isgraph((c = fgetc(lim.sourceFile))) && c != EOF);

		// get identifiers to:
		// variable; table; table key; function
		if(!isHex && ct_getIdentifier(&c, true)){
			do{
				// the dot in first will protect this
				// key against the compaction process
				// <table> . <ident> [(]
				if(c == '.')
					fputc('\n', extrCttBuf);

				fputc(c, extrCttBuf);

				// this parenthesis specify that
				// this identify belongs to function
				if(c == '(')
					break;

				c = fgetc(lim.sourceFile);
			}while(ct_getIdentifier(&c, false));

			fputc('\n', extrCttBuf);

			// without this condition, strings found after
			// a identifiers will be treated incorrectly
			if(c == '\'' || c == '"'){
				ct_saveString(extrCttBuf, c);
				fputc('\n', extrCttBuf);
				continue;
			}

			// if the characterer that stop the loop
			// is not pritend here, it will be lost;
			// this character is invalid to form
			// a identifier
			if(c != '(' && !isalnum(c) && isgraph(c)){
				fputc(c, extrCttBuf);
				fputc('\n', extrCttBuf);
			}

			continue;
		}

		// constant numbers: integer, float
		if(isdigit(c) || (isHex && isxdigit(c))){
			if(ct_hexTest(lim.sourceFile, extrCttBuf, &c, &isHex))
				continue;

			do{
				fputc(c, extrCttBuf);
				c = fgetc(lim.sourceFile);

				// float test
				if(!isHex && (c == '.' || isFloat))
					isFloat = !isFloat;

			}while(isdigit(c) || isFloat || (isHex && isxdigit(c)));

			isFloat = isHex = false;
			fputc('\n', extrCttBuf);
			fseek(lim.sourceFile, -1, SEEK_CUR);
			continue;
		}

		// strings, commentaries, table environment
		// special characteres (:,.{-+)
		ct_getSpecial(extrCttBuf, c);
		isHex = false;
	}

	fclose(lim.sourceFile);
	lim.sourceFile = NULL;

	if(flags.untilStage == 1){
		t_copyAndExportFile(extrCttBuf);
		return true;
	}

	return false;
}

// read content getted in last stage and separate
// them in:
// [DECLARATIONS OF] variables and tables (root environment)
// [~] functions (root environment)
// [~] global variables and tables
// [~] global functions
// [~] local variables and tables ("root function" environment)
// [~] declarations of local functions ("root function" environment)
// uses, or calls, of variables and tables, or functions
// functions from Lua (default set)
// functions from "List Partition", of "header.lim"
// values that will not be compacted (numbers, metamethods, special, ...)
// order in that the content strings were diffused in buffers
bool cp_2_separateExtractedContent(void){
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
	// "cursor" is inside a; it MUST NOT
	// BE freed, because its pointer is
	// stored in queue of function
	// environment, from buffer fromsrc
	char *funcName = NULL;

	mm_stringInit(&string);
	fromsrc_init();

	fseek(extrCttBuf, 0, SEEK_SET);

	while((c = fgetc(extrCttBuf)) != EOF){
		t_buildStringFromFile(extrCttBuf, &c, &string);

		// get and set content (string) type
		typeCode = TYPE_NONE;
		
		if(prefix != PREFIX_NONE)
			typeCode = ct_readPrefix(string, prefix, isRootEnv);

		if(typeCode == TYPE_NONE)
			typeCode = ct_readCurWord(string);

		prefix = ct_setPrefix(string, prefix, isRootEnv);

		// outside functions
		if(isRootEnv){
			funcName = ct_checkAndCreateNewEnv(string, typeCode);

			if(funcName != NULL){
				fromsrc_newEnv(funcName);
				codeBlockLayer++;
				isRootEnv = false;
			}
		// inside a function
		}else{
			ct_checkAndUpLayer(string, &codeBlockLayer);

			if(codeBlockLayer == 0){
				isRootEnv = true;
				isAnony = false;

				funcName = NULL;
			}
		}

		fromsrc_order(typeCode);
		fromsrc_print(string, funcName, typeCode);

		mm_stringEnd(&string, true);
	}

	free(anonyName);
	mm_stringEnd(&string, false);
	fclose(extrCttBuf);

	if(flags.untilStage == 2){
		GlobalEnv *from;

		const char* const titles[] = {
			"LIBRARY FUNCTIONS",
			"LIBRARY VARIABLES AND TABLES",
			"GLOBAL FUNCTIONS",
			"GLOBAL VARIABLES AND TABLES",
			"USE OR CALL",
			"CONSTANTS",
			"TABLES AND FUNCTIONS FROM Lua",
			"TABLES AND FUNCTIONS FROM \"header.lim\"",
		};

		from = fromsrc_get();
		toexport = tmpfile();
		fromsrc_fseekSetAll();

		fprintf(toexport, "@@ ROOT ENVIRONMENTS\n");

		// "order" file is jumpped, because
		// it is binary and it is not content
		for(short i = 1; i < FROMSRC_TOTAL_BUF; i++){
			fprintf(toexport, "@ %s\n", titles[i - 1]);
			t_fcat(from->bufs[i], toexport);
			fprintf(toexport, "\n\n");
		}

		FuncEnv *i;
		fprintf(toexport, "@@ FUNCTION ENVIRONMENTS\n@- [\"__fn_\" for anonymous functions]\n");
		for(i = from->head; i != NULL; i = i->next){
			fprintf(toexport, "@ FROM FUNCTION: %s\n- FUNCTIONS\n", i->name);
			t_fcat(i->bufs[0], toexport);
			fprintf(toexport, "\n- VARIABLES AND TABLES\n");
			t_fcat(i->bufs[1], toexport);
			fprintf(toexport, "\n\n");
		}
	
		t_copyAndExportFile(toexport);
		fromsrc_end();
		return true;
	}

	return false;
}

// use "cp_3_buildingGlobalScope_functions/variablesAndTable"
// to make the data structures: tree (temp.) and queue (x2);
// --until-stage: variables, tables and functions with they
// nicknames and quantity of use in code.
bool cp_3_buildingGlobalScope(void){
	cp_3_buildingGlobalScope_functions();
	cp_3_buildingGlobalScope_variablesAndTables();

	if(flags.untilStage == 3){
		toexport = tmpfile();

		fprintf(toexport, "@@ %s\n@@ %s\n@@ %s\n@@ %s\n\n@- %s\n\n",
			"THIS STAGE (3) DON'T CHANGE THE SOURCE CONTENT,",
			"IT SET A NICKNAME TO ALL THE \"VALID IDENTIFIERS\"",
			"PRESENT IN CODE; TO SEE THE LAST CHANGES IN THE",
			"SOURCE CONTENT TRY: `lim <source> -us 2`",
			"[QUANTITY]\t\t[NICKNAME]\t\t[IDENTIFIER]"
		);

		fprintf(toexport, "@ FUNCTIONS \n");
		pairs_printAll(toexport, false);

		fprintf(toexport, "\n@ VARIABLES AND TABLES\n");
		pairs_printAll(toexport, true);

		t_copyAndExportFile(toexport);
		pairs_end();
		return true;
	}

	return false;
}

// PART 1:
// make a reference tree with functions from
// Lua (default) and "header.lim". After it is
// finished, it is converted in a queue, ordered
// from bigger to lower, with this node structure
// (tree nodes and queue items have this structure):
// table    - math - math - NULL
// function - .rad - NULL - type
// quantity -   0  -  0   -  0
// PART 2:
// build reference score, to output file, with
// this structure:
// local A,B,C=math.rad,math,type
// PART 3:
// set a nickname to all "root/global functions"
// and its (aligned) variables, tables and functions;
// and add them in the buffer "pairs" (2).
static void cp_3_buildingGlobalScope_functions(void){
	//////////////////// PART 1 ////////////////////
	
	// length of "string", used to copy it
	// content to "table"
	unsigned short len;

	// math, string, table, ...; if it suffix will
	// not be valid, it will be treaty like "word"
	char *table = NULL;

	// lists of functions that will be to win a
	// reference (from Lua and from "header.lim")
	FILE *fileList[2] = {
		(fromsrc_get())->bufs[TYPE_FROM_LUA],
		(fromsrc_get())->bufs[TYPE_FROM_HEAD],
	};

	mm_stringInit(&string);
	refe_init();

	// count how many times a specify function, from
	// lua default or from "header.lim" was used
	for(short fileId = 0; fileId < 2; fileId++){
		if(fileList[ fileId ] == NULL)
			break;

		fseek(fileList[ fileId ], 0, SEEK_SET);

		while((c = fgetc(fileList[ fileId ])) != EOF){
			t_buildStringFromFile(fileList[ fileId ], &c, &string);

			// the string is a table
			if(string[0] != '.' && string[len - 1] != '(' && (fileId == 1 || ct_checkLuaTabs(string))){
				// a table was collected, but after it,
				// none function was placed
				if(table != NULL){
					refe_add(table, NULL);
					free(table);
				}

				len = strlen(string);
				table = malloc(len + sizeof(char));
				strcpy(table, string);
				mm_stringEnd(&string, true);
				continue;
			}

			// print the table finded before with
			// the function finded right now
			if(table != NULL){
				refe_add(table, string);
				free(table);
				table = NULL;
				mm_stringEnd(&string, true);
				continue;
			}

			// function no prefixed by a table
			// (tonumber, next, select, ...)
			refe_add(NULL, string);
			mm_stringEnd(&string, true);
		}

		if(table != NULL){
			refe_add(NULL, table);
			free(table);
			table = NULL;
		}
	}

	mm_stringEnd(&string, false);
	refe_treeToQueue();
	refe_endTree();

	//////////////////// PART 2 ////////////////////

	scope_init();
	nick_init(true);
	pairs_init();

	// used to merge a table with it key:
	// math + .random = math.random
	char *fullContent;

	// temporary store content to merge,
	// that will be stored in "fullContent"
	char *fullCttBuf;

	// current item gettend from
	// buffer "refe_queue"
	Queue *item;

	scope_add("local ", SCOPE_FUNC);
	scope_rmvLastComma(SCOPE_FUNC);

	// all references are readed and classified, now they need
	// to be positioned in a "Lua declaration structure":
	// local ident0,ident1,identn=value0,value1,valuen
	while((item = refe_getAndRmvQueueItem()) != NULL){
		// get full content from queue item
		fullContent = malloc(sizeof(char));
		fullContent[0] = '\0';

		// get table
		if(item->content[CTT_TABLE] != NULL){
			fullCttBuf = malloc(strlen(item->content[CTT_TABLE]) + 1);
			strcpy(fullCttBuf, item->content[CTT_TABLE]);

			free(fullContent);
			fullContent = fullCttBuf;
		}

		// get function (and merge)
		if(item->content[CTT_FUNC] != NULL){
			fullCttBuf = malloc(strlen(fullContent) + strlen(item->content[CTT_FUNC]) + 1);
			strcpy(fullCttBuf, fullContent);
			strcat(fullCttBuf, item->content[CTT_FUNC]);

			free(fullContent);
			fullContent = fullCttBuf;
		}

		// print content getted from
		// item in specify buffers
		scope_add(nick_get(), SCOPE_FUNC);
		scope_add(fullContent, SCOPE_ADDR);

		pairs_add(false, item->quantity, nick_get(), fullContent);

		// free dinamic memories
		mm_treeFreeNodeAndQueueItem(NULL, item);
		free(fullContent);
		nick_up();
	}

	//////////////////// PART 3 ////////////////////

	// current function getted from
	// "functions"; it is used for to
	// get specify function environment
	char *bufName = NULL;

	// index of the function
	// environment
	short fenvId;

	// current function environment:
	// variables and tables
	// functions
	FILE *fenv;

	// list of functions declared
	// in "root environment"
	FILE *functions;

	// "l" + `nick_get`
	char *localnick;

	functions = fromsrc_getBuf(TYPE_GLOBAL_FUNC, NULL);
	fseek(functions, 0, SEEK_SET);

	nick_end();
	nick_init(false);

	mm_stringInit(&string);
	mm_stringInit(&bufName);

	// yes, their nickname are in uppercase
	while((c = fgetc(functions)) != EOF){
		t_buildStringFromFile(functions, &c, &bufName);

		for(fenvId = TYPE_LOCAL_FUNC_0; fenvId <= TYPE_LOCAL_VAR_1; fenvId++){
			fenv = fromsrc_getBuf(fenvId, bufName);
			fseek(fenv, 0, SEEK_SET);

			while((c = fgetc(fenv)) != EOF){
				t_buildStringFromFile(fenv, &c, &string);

				localnick = malloc(strlen(nick_get()) + (sizeof(char) * 2));
				strcpy(localnick, "l");
				strcat(localnick, nick_get());

				scope_localAdd(bufName, localnick);
				pairs_add(true, 0, localnick, string);

				mm_stringEnd(&string, true);
				nick_up();
				free(localnick);
			}
		}

		pairs_add(true, 0, nick_get(), bufName);
		nick_up();
		mm_stringEnd(&bufName, true);
	}

	// merge functions values (addre)
	// with their references (func)
	if(t_filelen(scope_get(SCOPE_ADDR)) > 0){
		scope_localEnd();
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
		t_fcat(src, dest);

		scope_rmvLastComma(SCOPE_FUNC); // "it" == dest
		fputc('\n', dest);
	}else{
	}

	// actually, "refe (queue)" is
	// ended during the build process
	mm_stringEnd(&string, false);
	mm_stringEnd(&bufName, false);
	nick_end();
}

// PART 1:
// make a reference queue with "global" variables
// and tables, created in source file, with the same
// structure of the queue in top.
// PART 2:
// read "use or call" buffer, count how many times each
// variables, or table, is used and updated the "quantity"
// in queue. After this, the queue is ordenated, from
// bigger to lower.
static void cp_3_buildingGlobalScope_variablesAndTables(void){
	//////////////////// PART 1 ////////////////////
	
	FILE *variables;
	variables = fromsrc_getBuf(TYPE_GLOBAL_VAR, NULL);
	fseek(variables, 0, SEEK_SET);

	mm_stringInit(&string);
	nick_init(false);

	scope_add("local ", SCOPE_VAR);
	scope_rmvLastComma(SCOPE_VAR);
	
	// read declarations of variables and tables
	while((c = fgetc(variables)) != EOF){
		t_buildStringFromFile(variables, &c, &string);

		pairs_add(true, 0, nick_get(), string);

		mm_stringEnd(&string, true);
		scope_add(string, SCOPE_VAR);
		nick_up();
	}

	if(t_filelen(scope_get(SCOPE_VAR)) > 0){
		scope_rmvLastComma(SCOPE_VAR);
	}else{
	}

	//////////////////// PART 2 ////////////////////
	
	short orderCode;
	fromsrc_fseekSetAll();
	mm_stringEnd(&string, true);
	fromsrc_fseekSetAll();

	// read uses of variables and tables
	while(fromsrc_getOrder(&orderCode)){
		if(orderCode != TYPE_USE_OR_CALL)
			continue;

		c = '\0';
		t_buildStringFromFile( (fromsrc_get())->bufs[orderCode], &c, &string);

		pairs_updateQuantity(string);
		mm_stringEnd(&string, true);
	}
	
	pairs_updateOrder();
	mm_stringEnd(&string, false);
}

bool cp_4_organizeAndCompact(void){}
void cp_5_mergingContentAndPackingLibrary(void){}
