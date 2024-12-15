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

	if(lim.sourceFile == NULL){
		er_nonExistentFile(lim.sourceFileName);
	}


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
}

bool cp_1_extractSourceContent(void){
	info_verbose(VM_STAGE, 1, "extract content from source file.");


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


		// all that start with a number is a number: 1xyz, 0.lua, ...
		if(isdigit(c)){
			do{
				fputc(c, extrCttBuf);
				c = fgetc(lim.sourceFile);
			}while(c != EOF && (isalnum(c) || c == '.' || c == '_'));

			if(c == EOF)
				break;

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

	
	return sp_extractSourceContent(1, extrCttBuf);
}

bool cp_2_separateExtractContent(void){
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


	info_verbose(VM_PROCESS, "separation");
	while((c = fgetc(extrCttBuf)) != EOF){
		t_getStringFromFile(extrCttBuf, &c, &string);

		// pre-set destine buffer (in fromsrc)
		typeCode = TYPE_NONE;
		
		if(prefix != PREFIX_NONE)
			typeCode = ct_readPrefix(string, prefix, blockLayer, isRootEnv);

		if(typeCode == TYPE_NONE)
			typeCode = ct_readCurWord(string);

		prefix = ct_setPrefix(string, prefix, typeCode, isRootEnv);

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
		fromsrc_write(string, funcName, typeCode);

		mm_stringEnd(&string, true);
	}


	info_verbose(VM_FREE, NULL);
	free(anonyName);
	mm_stringEnd(&string, false);

	info_verbose(VM_BUFFER_END, "extrCttBuf", NULL);
	fclose(extrCttBuf);


	return sp_separateExtractContent(2);
}

bool cp_3_globalScopeTo_varFunc(void){
	info_verbose(VM_STAGE, 3, "build scope to \"private global\" variables, tables and functions");


	// FUNCTIONS AND TABLES FROM LUA AND HEADER
	
	// math/string/table==treaty like "word"
	char *table = NULL;

	// from Lua/"header.lim"
	FILE *funcList[2] = {
		(fromsrc_get())->bufs[TYPE_FROM_LUA],
		(fromsrc_get())->bufs[TYPE_FROM_HEAD],
	};


	info_verbose(VM_BUFFER_INIT, "refe", NULL);
	refe_init();

	mm_stringInit(&string);


	// build binary tree with functions, from Lua
	// and header file, that were used in source file
	info_verbose(VM_NORMAL, "Build of binary tree (with \"root functions\")");
	for(short funcListId = 0; funcListId < 2; funcListId++){
		// only #1 can be NULL
		if(funcList[ funcListId ] == NULL)
			break;

		fseek(funcList[ funcListId ], 0, SEEK_SET);


		while((c = fgetc(funcList[ funcListId ])) != EOF){
			t_getStringFromFile(funcList[ funcListId ], &c, &string);

			// the string is a table
			if(string[0] != '.' && (funcListId == 1 || ct_checkLuaTabs(string))){
				// a table was collected, but after
				// it there is not function
				if(table != NULL){
					refe_add(table, NULL);
					free(table);
				}

				table = t_allocAndCopy(string);
				strcpy(table, string);
				mm_stringEnd(&string, true);
				continue;
			}

			if(table != NULL){
				refe_add(table, string);
				
				free(table);
				table = NULL;
				
				mm_stringEnd(&string, true);
				continue;
			}

			// function not prefixed by a table
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

	info_verbose(VM_FREE, NULL);
	mm_stringEnd(&string, false);

	info_verbose(VM_NORMAL, "Converting binary tree to queue...");
	refe_initQueueAndEndTree();


	Queue *item;

	unsigned short len;

	char *content[2], *fullContent, *firstToCopy;
	char *nickPrefixed;


	info_verbose(VM_BUFFER_INIT, "scope", "nick", "pairs", NULL);
	scope_init();
	nick_init(true);
	pairs_init;


	// add queue content to scope
	info_verbose(VM_PROCESS, "fragmentation of the reference queue (+default func. scope)");
	while((item = refe_getAndRmvQueueItem()) != NULL){
		len = 0;

		for(short i = 0; i < 2; i++){
			content[i] = NULL;

			if(item->content[i] != NULL){
				len = strlen(item->content[i]);
				content[i] = item->content[i];
			}

		}

		fullContent = malloc(len + 1);
		firstToCopy = (char*)((content[0] != NULL) ? content[0] : content[1]);

		strcpy(fullContent, firstToCopy);
		if(firstToCopy != content[1] && content[1] != NULL)
			strcat(fullContent, content[1]);

		nickPrefixed = nick_get(NICK_TO_LUA_HEAD_FUNC);

		scope_add(nickPrefixed, SCOPE_BASE);
		scope_add(fullContent, SCOPE_FUNC_BUF);
		pairs_add(false, item->quantity, nickPrefixed, fullContent);

		nick_up();
		free(fullContent);
		mm_treeFreeNodeAndQueueItem(NULL, item);
	}


	info_verbose(VM_BUFFER_END, "nick", NULL);
	nick_end();




	// VARIABLES AND TABLES FROM SOURCE FILE

	FILE *tmp;
	tmp = (fromsrc_get())->bufs[TYPE_GLOBAL_VAR];


	mm_stringInit(&string);

	info_verbose(VM_BUFFER_INIT, "nick", NULL);
	nick_init(false);


	// get "private global" variables and tables
	// identifier and ordenate to build them scope
	info_verbose(VM_PROCESS, "get \"private global\" variables and tables");
	for(short mode = 0; mode < 2; mode++){
		fseek(tmp, 0, SEEK_SET);

		while((c = fgetc(tmp)) != EOF){
			t_getStringFromFile(tmp, &c, &string);

			if(mode == 0)
				if(pairs_add(true, 0, nick_get(NICK_TO_GLOBAL_IDENT), string))
					nick_up();
			else
				pairs_updateQuantity(string);

			mm_stringEnd(&string, true);
		}
	}


	info_verbose(VM_NORMAL, "Updating order of variables and tables pairs...");
	pairs_updateOrder();

	// build their scope
	info_verbose(VM_PROCESS, "build of \"private global\" variables and tables scope");
	for(item = pairs_get(true); item != NULL; item = item->next)
		scope_add(item->content[0], SCOPE_BASE);


	info_verbose(VM_NORMAL, "Merging scope identifier with their values (address)...");
	SCOPE_MERGE_BASE_WITH_BUF;

	info_verbose(VM_FREE, NULL);
	mm_stringEnd(&string, false);

	info_verbose(VM_BUFFER_END, "nick", NULL);
	nick_end();

	return sp_globalScopeTo_varFunc(3);
}

bool cp_4_localScopeTo_varFuncGParPar(void){
	info_verbose(VM_STAGE, 4, "WiP");


	FILE *temp;
	FuncEnv *cur;
	short nickTypes[4] = {
		NICK_TO_LOCAL_IDENT, // variables and tables
		NICK_TO_LOCAL_IDENT, // functions
		NICK_TO_PARAMETER,   // parameters
		NICK_TO_ALIGN_PARAM, // parameters of aligned functions
	};


	info_verbose(VM_BUFFER_INIT, "local", NULL);
	local_init();
	mm_stringInit(&string);


	info_verbose(VM_PROCESS, "creation of nicknames to \"root functions\"");
	for(cur = local_get(); cur != NULL; cur = cur->next){
		// "i WANT that the variables and
		// tables buffer are the FIRST!"
		// 0(1): functions; 1(0): variables and tables
		temp = cur->bufs[0];
		cur->bufs[0] = cur->bufs[1];
		cur->bufs[1] = temp;

		for(short i = 0; i < 4; i++){

			// func. "continue" variables nickname sequence
			if(i != 1)
				nick_init(false);

			fseek(cur->bufs[i], 0, SEEK_SET);

			while((c = fgetc(cur->bufs[i])) != EOF){
				t_getStringFromFile(cur->bufs[i], &c, &string);

				if(local_pairsAdd(&(cur->pairs), nick_get(nickTypes[i]), string))
					nick_up();

				mm_stringEnd(&string, true);
			}

			// only variables and tables
			if(i == 0){
				Queue *item;

				for(item = cur->pairs; item != NULL; item = item->next)
					local_scopeAdd(cur->scope, item->content[0]);

				local_scopeRmvLastComma(cur);
			}

			if(i > 0)
				nick_end();
		}

		local_pairsUpdateOrder(cur);
	}

	info_verbose(VM_FREE, NULL);
	mm_stringEnd(&string, false);

	return sp_localScopeTo_varFuncGParPar(4);
}

bool cp_5_organizeAndCompact(void){
	fromsrc_end();
	scope_end();
	pairs_end();
	local_end();

	return false;
}

void cp_6_mergeContentAndPackLib(void){
	// header.lim
	//info_verbose(VM_NORMAL, "Loading \"header.lim\":...");
	//info_verbose(VM_NORMAL, header_init());

	//header_end();
}
