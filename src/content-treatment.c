#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "heads.h"

static char _c;
static char *anonyName = NULL;
static char *commoName = NULL;
static const char lua_keywords[21][9] = {"do","if","in","or","and","end","for","nil","not","else","then","true","break","false","local","until","while","elseif","repeat","return","function"};
static const char lua_funcs[23][15] = {"print","tonumber","tostring","ipairs","type","pairs","assert","error","setmetatable","getmetatable","rawset","rawget","rawlen","rawequal","pcall","xpcall","select","next","collectgarbage","require","load","dofile","loadfile"};


void ct_atexit(void){
	free(anonyName);
	free(commoName);
}



////////// STAGE 1 //////////

void ct_specialCharTreatment(FILE *buf, char c){
	// COMMENTARIES
	// line
	if(c == '-'){
		if(fgetc(lim.sourceFile) == '-'){
		
			// block
			if(fgetc(lim.sourceFile) == '['){
				if(fgetc(lim.sourceFile) == '['){
					clearComment(false);
					return;
				}
				fseek(lim.sourceFile, -1, SEEK_CUR);
			}

			fseek(lim.sourceFile, -1, SEEK_CUR);
			clearComment(true);
			return;
		}
		fseek(lim.sourceFile, -1, SEEK_CUR);
	}

	// STRINGS
	_c = c;
	if(c == '\'' || c == '"'){
		saveString(buf, c);
		fputc('\n', buf);
		return;
	}

	// TABLE ENVIRONMENT
	if(c == '{'){
		saveBraces(buf);
		return;
	}

	// SPECIAL CHARACTERS
	fputc(c, buf);
	fputc('\n', buf);
}

static void saveString(FILE *buf, char signal){
	short invBar = 2; // '\'
	char c = signal;

	do{
		if(c != '\n' && c != '\t')
			fputc(c, buf);

		// check inverted bar:
		// this annul the possible next
		// single/double quotation mark
		if(invBar > 0)
			invBar--;
		else if(c == '\\')
			invBar = 2;

	}while((c = fgetc(lim.sourceFile)) != EOF && (c != signal || invBar == 1));

	// the quotation mark that
	// break the loop
	fputc(signal, buf);
}

static void clearComment(bool isLine){
	if(isLine){
		fseek(lim.sourceFile, -2, SEEK_CUR); // remove `--` until '\n'
		
		while((_c = fgetc(lim.sourceFile)) != EOF && _c != '\n');
		return;
	}

	fseek(lim.sourceFile, -4, SEEK_CUR); // remove `--[[` until `]]`

	while((_c = fgetc(lim.sourceFile)) != EOF)
		if(_c == ']')
			if(fgetc(lim.sourceFile) == ']')
				break;
}

static void saveBraces(FILE *buf){
	unsigned long qtt = 1;
	bool cond0, cond1;


	char before = '{';
	char current = _c;
	char after;

	while(qtt > 0 && current != EOF){
		after = fgetc(lim.sourceFile);
		fseek(lim.sourceFile, -1, SEEK_CUR);

		if(isgraph(current) &&  current != '\'' && current != '"')
			fputc(current, buf);
		
		cond0 = (isalnum(before) &&            current == '\n'          && !isalnum(after));
		cond1 = (isalnum(before) && (current == ' ' || current == '\t') &&  isalnum(after));
		
		if(cond0 || cond1)
			fputc(' ', buf);

		before = current;
		current = fgetc(lim.sourceFile);

		if(current == '{')
			qtt++;
		else if(current == '}')
			qtt--;
		else if(current == '\'' || current == '"')
			saveString(buf, current);
	}

	fputc('}', buf);
	fputc('\n', buf);
}



////////// STAGE 2 //////////

short ct_readPrefix(char *word, short prefix, short blockLayer, bool isRootEnv){
	if(prefix == PREFIX_PARAMETER){
		if(t_strcmp2(word, ",", "("))
			return TYPE_PARAM_CONST;

		// end of function parameters
		if(strcmp(word, ")") == 0)
			return TYPE_PARAM_END;

		if(blockLayer == 1) // itself parameters
			return TYPE_LOCAL_PSELF_2; // lib and global

		// from aligned functions
		return TYPE_LOCAL_PALIG_3;
	}

	if(prefix == PREFIX_LOCAL && strcmp(word, "function") == 0)
		return TYPE_CONSTANT;

	if(prefix == PREFIX_FUNCTION && strcmp(word, "(") == 0)
		return TYPE_ANONYMOUS;

	switch(prefix){
		case PREFIX_G:          return TYPE_LIB_VAR;
		case PREFIX_FUNCTION:   return TYPE_LIB_FUNC;
		case PREFIX_LOCAL_FUNC: return ((isRootEnv) ? TYPE_GLOBAL_FUNC : TYPE_LOCAL_FUNC_0);
		case PREFIX_LOCAL:      return ((isRootEnv) ? TYPE_GLOBAL_VAR  : TYPE_LOCAL_VAR_1);
		case PREFIX_LUA_TABLE:  return ((word[0] == '.') ? TYPE_FROM_LUA : TYPE_NONE);
		default: return TYPE_NONE;
	}
}

short ct_readCurWord(char *word){
	if(isalpha(word[0]) || word[0] == '_'){
		// metamethods; from lua
		if(word[1] == '_' || checkLuaKeywords(word, false))
			return TYPE_CONSTANT;

		if(checkLuaFuncs(word) || ct_checkLuaTabs(word))
			return TYPE_FROM_LUA;

		if(header_checkFuncList(word))
			return TYPE_FROM_HEAD;

		// use of variables and tables;
		// and function call
		return TYPE_USE_OR_CALL;
	}

	// numbers, special
	// characteres, table keys
	return TYPE_CONSTANT;
}

short ct_setPrefix(char *word, short prefix, short codeType, bool isRootEnv){
	if(codeType == TYPE_PARAM_END)
		return PREFIX_NONE;

	if(prefix == PREFIX_PARAMETER)
		return prefix;

	if(IS_FUNC_TYPE(codeType))
		return PREFIX_PARAMETER;

	if(strcmp(word, "_G") == 0)
			return PREFIX_G;

	if(strcmp(word, "local") == 0)
		return PREFIX_LOCAL;

	if(strcmp(word, "function") == 0)
		if(prefix == PREFIX_LOCAL)
			return PREFIX_LOCAL_FUNC;
		else
			return PREFIX_FUNCTION;
	
	if(ct_checkLuaTabs(word))
		return PREFIX_LUA_TABLE;

	return PREFIX_NONE;
}

char* ct_checkAndCreateNewEnv(char *word, short typeCode, short *anonyId){
	if(typeCode == TYPE_ANONYMOUS){
		free(anonyName);
		free(commoName);
		commoName = NULL;

		anonyName = t_setAnonyFuncName(anonyId);

		return anonyName;
	}

	if(typeCode == TYPE_LIB_FUNC || typeCode == TYPE_GLOBAL_FUNC || typeCode == TYPE_LOCAL_FUNC_0){
		free(anonyName);
		free(commoName);
		anonyName = NULL;

		commoName = malloc(strlen(word) + sizeof(char));
		strcpy(commoName, word);

		return commoName;
	}

	return NULL;
}

void ct_checkAndUpLayer(char *word, unsigned short *code){
	if(strcmp(word, "if") == 0 || strcmp(word, "do") == 0 || strcmp(word, "function") == 0)
		(*code)++;
	else if(*code > 0 && strcmp(word, "end") == 0)
		(*code)--;
}

static bool checkLuaKeywords(char *word, bool stage1){
	// with(out) `function`
	unsigned short max = ((stage1) ? 20 : 21);

	for(short i = 0; i < max; i++)
		if(strcmp(word, lua_keywords[i]) == 0)
			return true;

	return false;
}

static bool checkLuaFuncs(char *word){
	for(short i = 0; i < 23; i++)
		if(strcmp(word, lua_funcs[i]) == 0)
			return true;

	return false;
}

bool ct_checkLuaTabs(char *word){
	for(short i = 0; i < 9; i++)
		if(strcmp(word, lim.lua_tabs[i]) == 0)
			return true;

	return false;
}
