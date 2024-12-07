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

void cp_0_checkAndOpenFiles(void){
	info_verbose(VM_STAGE, 0, "check and open file specified.");


	info_verbose(VM_NORMAL, "Checking source code file...");
	lim.sourceFile = fopen(lim.sourceFileName, "r");

	if(lim.sourceFile == NULL)
		er_nonExistentFile(lim.sourceFileName);


	if(flags.replace){
		info_verbose(VM_NORMAL, "REPLACE: ON =x");

	}else{
		info_verbose(VM_NORMAL, "REPLACE: OFF => checking if output file already exist...");

		FILE *dst;
		dst = fopen(lim.destineFileName, "r");

		if(dst != NULL){
			fclose(dst);
			er_fileAlreadyExistent(lim.destineFileName);
		}
	}


	// header.lim
	//info_verbose(VM_NORMAL, "Loading \"header.lim\":...");
	//info_verbose(VM_NORMAL, head_init());
}

bool cp_1_extractionFromOrigin(void){
	info_verbose(VM_STAGE, 1, "extract content from source file.");


	// ==1: valid
	short isFloat = 0;


	info_verbose(VM_BUFFER_INIT, "extrCttBuf", NULL);
	extrCttBuf = tmpfile();


	info_verbose(VM_PROCESS, "extraction");
	while((c = fgetc(lim.sourceFile)) != EOF){
		if(!isgraph(c))
			while(!isgraph((c = fgetc(lim.sourceFile))) && c != EOF);

		
		// variable; table; table key; function
		if(c == '_' || isalpha(c)){
			do{
				// table. \n ident0. \n identn.;
				if(c == '.' || c == ':')
					SEPARATOR(extrCttBuf);

				fputc(c, extrCttBuf);

				// it is a function
				if(c == '(')
					break;

				c = fgetc(lim.sourceFile);
			}while(c != EOF && (isalnum(c) || c == '_' || c == ':' || c == '.'));

			if(c == EOF)
				break;

			fseek(lim.sourceFile, -1, SEEK_CUR);
			SEPARATOR(extrCttBuf);
			continue;
		}


		// all that start with a number is a number: 1xyz, 0xgh, ...
		if(isdigit(c)){
			do{
				fputc(c, extrCttBuf);
				c = fgetc(lim.sourceFile);

				if(c == '.' && isFloat == 0)
					isFloat = 1;
				else if(isFloat == 1)
					isFloat = -1;

			}while(c != EOF && (isalnum(c) || isFloat == 1 || c == '_'));

			if(c == EOF)
				break;

			isFloat = 0;
			fseek(lim.sourceFile, -1, SEEK_CUR);
			SEPARATOR(extrCttBuf);
			continue;
		}

		// strings; commentaries; table environment; special char.
		ct_specialCharTreatment(extrCttBuf, c);
	}


	info_verbose(VM_NORMAL, "Closing source file...");
	fclose(lim.sourceFile);
	lim.sourceFile = NULL;

	
	return stageProduct_extractionFromOrigin(1, extrCttBuf);
}

bool cp_2_separateExtractedContent(void){
	info_verbose(VM_STAGE, 2, "separate extracted content.");


	bool isRootEnv = true, isFunc = false, isAnony = false;
	short prefix = PREFIX_NONE, typeCode = TYPE_NONE;
	char *funcName = NULL, *anonyName = NULL;
	unsigned short blockLayer = 0, anonyId = 0;


	info_verbose(VM_BUFFER_INIT, "fromsrc", NULL);
	fromsrc_init();

	mm_stringInit(&string);

	info_verbose(VM_NORMAL, "Setting \"extrCttBuf\" cursor to outset...");
	fseek(extrCttBuf, 0, SEEK_SET);


	info_verbose(VM_STAGE, "separation");
	while((c = fgetc(extrCttBuf)) != EOF){
		t_getStringFromFile(extrCttBuf, &c, &string);

		// pre-set destine buffer (in fromsrc)
		typeCode = TYPE_NONE;
		
		if(prefix != PREFIX_NONE)
			typeCode = ct_readPrefix(string, prefix, isRootEnv);

		if(typeCode == TYPE_NONE)
			typeCode = ct_readCurWord(string);

		prefix = ct_setPrefix(string, prefix, isRootEnv);

		// root environment
		if(isRootEnv){
			funcName = ct_checkAndCreateNewEnv(string, typeCode, &anonyId);

			if(funcName != NULL){
				fromsrc_newEnv(funcName);
				blockLayer++;
				isRootEnv = false;
			}
		// root function environment
		}else{
			ct_checkAndUpLayer(string, &blockLayer);

			if(blockLayer == 0){
				isRootEnv = true;
				isAnony = false;

				funcName = NULL;
			}
		}

		fromsrc_order(typeCode);
		fromsrc_print(string, funcName, typeCode);

		mm_stringEnd(&string, true);
	}


	info_verbose(VM_FREE, NULL);
	free(anonyName);
	mm_stringEnd(&string, false);

	info_verbose(VM_BUFFER_END, "extrCttBuf", NULL);
	fclose(extrCttBuf);


	return stageProduct_separateExtractedContent(2);
}

// use "cp_3_buildingGlobalScope_functions/variablesAndTable"
// to make the data structures: tree (temp.) and queue (x2);
// --until-stage: variables, tables and functions with they
// nicknames and quantity of use in code.
bool cp_3_buildingGlobalScope(void){
	/*
	cp_3_buildingGlobalScope_functions();
	cp_3_buildingGlobalScope_variablesAndTables();

	return stageProduct_buildingGlobalScope(3);
	*/
	return false;
}
/*
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
			t_getStringFromFile(fileList[ fileId ], &c, &string);

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
		t_getStringFromFile(functions, &c, &bufName);

		for(fenvId = TYPE_LOCAL_FUNC_0; fenvId <= TYPE_LOCAL_VAR_1; fenvId++){
			fenv = fromsrc_getBuf(fenvId, bufName);
			fseek(fenv, 0, SEEK_SET);

			while((c = fgetc(fenv)) != EOF){
				t_getStringFromFile(fenv, &c, &string);

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
		t_getStringFromFile(variables, &c, &string);

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
		t_getStringFromFile( (fromsrc_get())->bufs[orderCode], &c, &string);

		pairs_updateQuantity(string);
		mm_stringEnd(&string, true);
	}
	
	pairs_updateOrder();
	mm_stringEnd(&string, false);
}
*/
bool cp_4_organizeAndCompact(void){}
void cp_5_mergingContentAndPackingLibrary(void){}
