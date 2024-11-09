#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "heads.h"

// to `ct_getSpecial`
// and its process
static char _c;

static const char lua_keywords[21][9] = {"do","if","in","or","and","end","for","nil","not","else","then","true","break","false","local","until","while","elseif","repeat","return","function"};
static const char lua_funcs[23][15] = {"assert","next","require","collectgarbage","pairs","select","dofile","pcall","getmetatable","error","print","tonumber","setmetatable","rawequal","tostring","ipairs","rawget","type","load","rawlen","xpcall","loadfile","rawset"};
static const char lua_tabs[9][10] = {"bit32","courotine","debug","io","math","os","package","string","table"};



////////// STAGE 1 //////////

bool ct_getIdentifier(char *c, bool isFirst){
	if(*c == ' '){
		// to search the special character
		// more closeness
		*c = ct_clearSpaces();
		
		// space between different words
		if(isalpha(*c) != 0 || *c == '_' || isdigit(*c)){
			fseek(gf_origin, -1, SEEK_CUR);
			return false;
		}
	}

	// ([a-zA-Z] or '_') or (!isF and ([0-9] or '.' or '(' or ':'))
	return (isalpha(*c) != 0 || *c == '_' || (!isFirst && (isdigit(*c) || *c == '.' || *c == '(' || *c == ':')));
}

char ct_clearSpaces(void){
	char c;
	while((FGETC == ' ' || c == '\t')  && c != EOF);
	return c;
}

void ct_saveString(char signal){
	bool invBar = false; // '\'
	char c = signal;

	do{
		if(c != '\n' && c != '\t')
			collect_add(c);

		if(invBar)
			invBar = false;
		else if(c == '\\')
			invBar = true;

	}while((c = fgetc(gf_origin)) != EOF && c != signal);

	collect_add(c);
	collect_add('\n');
}

void ct_getSpecial(char c){
	// COMMENTARIES
	// line
	if(c == '-'){
		if(fgetc(gf_origin) == '-'){
		
			// block
			if(fgetc(gf_origin) == '['){
				if(fgetc(gf_origin) == '['){
					clearComment(false);
					return;
				}
				fseek(gf_origin, -1, SEEK_CUR);
			}

			fseek(gf_origin, -1, SEEK_CUR);
			clearComment(true);
			return;
		}
		fseek(gf_origin, -1, SEEK_CUR);
	}

	// STRINGS
	_c = c;
	if(c == '\'' || c == '"'){
		ct_saveString(c);
		return;
	}

	// table BRACES
	if(c == '{'){
		saveBraces();
		return;
	}

	// DOUBLE SIGNAL
	if(saveDoubleSignal('=', '=')) return;
	if(saveDoubleSignal('/', '/')) return;
	if(saveDoubleSignal('.', '.')) return;
	if(saveDoubleSignal('>', '=')) return;
	if(saveDoubleSignal('<', '=')) return;

	// OTHER
	collect_add(c);
	collect_add('\n');
}

static void clearComment(bool isLine){
	if(isLine){
		fseek(gf_origin, -2, SEEK_CUR); // remove '--'
		
		while((_c = fgetc(gf_origin)) != EOF && _c != '\n');
		return;
	}

	fseek(gf_origin, -4, SEEK_CUR); // remove '--[['

	while((_c = fgetc(gf_origin)) != EOF)
		if(_c == ']')
			if(fgetc(gf_origin) == ']')
				break;
}

static void saveBraces(void){
	unsigned int qtt = 1;

	char before = '{';
	char after;

	bool cond0, cond1;

	while(qtt > 0 && _c != EOF){
		after = fgetc(gf_origin);
		fseek(gf_origin, -1, SEEK_CUR);

		if(isgraph(_c))
			collect_add(_c);
		
		cond0 = (_c == '\n' && isalnum(before) && !isalnum(after));
		cond1 = ((_c == ' ' || _c == '\t') && isalnum(after) && isalnum(before));
		
		if(cond0 || cond1)
			collect_add(' ');

		before = _c;

		_c = fgetc(gf_origin);

		if(_c == '{')
			qtt++;
		else if(_c == '}')
			qtt--;
		else if(_c == '\'' || _c == '"');
			//ct_saveString(_c);
	}

	collect_add('}');
	collect_add('\n');
}

static bool saveDoubleSignal(char sig_0, char sig_1){
	if(_c == sig_0){
		if(fgetc(gf_origin) == sig_1){
			collect_add(sig_0);
			collect_add(sig_1);
			collect_add('\n');

			return true;
		}

		fseek(gf_origin, -1, SEEK_CUR);
	}
	
	return false;
}



////////// STAGE 2 //////////

short readPrefix(char *word, short prefix, bool isRootEnv){
	if(prefix == PREFIX_LOCAL && strcmp(word, "function") == 0)
		return TYPE_CONSTANT;

	switch(prefix){
		case PREFIX_G:          return TYPE_LIB_VAR;
		case PREFIX_FUNCTION:   return TYPE_LIB_FUNC;
		case PREFIX_LOCAL_FUNC: return ((isRootEnv) ? TYPE_GLOBAL_FUNC : TYPE_LOCAL_FUNC);
		case PREFIX_LOCAL:      return ((isRootEnv) ? TYPE_GLOBAL_VAR  : TYPE_LOCAL_VAR);
		case PREFIX_LUA_TABLE:  return TYPE_FROM_LUA;
		default: return TYPE_NONE;
	}
}

short readCurWord(char *word){
	if(isalpha(word[0]) || word[0] == '_'){
		// metamethods
		if(word[1] == '_')
			return TYPE_CONSTANT;

		// anonymous function
		if(strcmp(word, "function(") == 0)
			return TYPE_ANONYMOUS;

		// from lua
		if(checkLuaKeywords(word))
			return TYPE_CONSTANT;

		if(checkLuaFuncs(word) || checkLuaTabs(word))
			return TYPE_FROM_LUA;

		// use of variables and tables;
		// and function call
		return TYPE_USE_OR_CALL;
	}

	// numbers, special
	// characteres, table keys
	return TYPE_CONSTANT;
}

short setPrefix(char *word, short prefix, bool isRootEnv){
	if(strcmp(word, "_G") == 0)
		return PREFIX_G;

	if(strcmp(word, "local") == 0)
		return PREFIX_LOCAL;

	if(strcmp(word, "function") == 0)
		if(prefix == PREFIX_LOCAL)
			return PREFIX_LOCAL_FUNC;
		else
			return PREFIX_FUNCTION;
	
	if(checkLuaTabs(word))
		return PREFIX_LUA_TABLE;

	return PREFIX_NONE;
}

char* checkAndCreateNewEnv(char *word, short typeCode){
	static char *anonyName = NULL;
	static char *commoName = NULL;
	static unsigned short anonyId = 0;

	if(typeCode == TYPE_ANONYMOUS){
		free(anonyName);
		free(commoName);
		commoName = NULL;

		anonyName = malloc(strlen(word) + 1 + INT_LEN(anonyId++));

		sprintf(anonyName, "%s%u", word, anonyId);

		return anonyName;
	}

	if(typeCode == TYPE_LIB_FUNC || typeCode == TYPE_GLOBAL_FUNC || typeCode == TYPE_LOCAL_FUNC){
		free(anonyName);
		free(commoName);
		anonyName = NULL;

		commoName = malloc(strlen(word) + 1);
		strcpy(commoName, word);

		return commoName;
	}

	return NULL;
}

void checkAndUpLayer(char *word, unsigned short *code){
	if(strcmp(word, "if") == 0 || strcmp(word, "do") == 0 || strcmp(word, "function") == 0)
		*code = *code + 1;
	else if(*code > 0 && strcmp(word, "end") == 0)
		*code = *code - 1;
}

static bool checkLuaKeywords(char *word){
	for(short i = 0; i < 21; i++)
		if(strcmp(word, lua_keywords[i]) == 0)
			return true;

	return false;
}

static bool checkLuaFuncs(char *word){
	unsigned short len = strlen(word);
	char *tocmp = word;

	if(word[len - 1] == '('){
		tocmp = malloc(len + 1);
		strcpy(tocmp, word);

		// replace '('
		tocmp[len - 1] = '\0';

		// to use `free`
		len = -1;
	}

	for(short i = 0; i < 21; i++){
		if(strcmp(tocmp, lua_funcs[i]) == 0){
			if(len == -1)
				free(tocmp);

			return true;
		}
	}

	if(len == -1)
		free(tocmp);

	return false;
}

static bool checkLuaTabs(char *word){
	for(short i = 0; i < 21; i++)
		if(strcmp(word, lua_tabs[i]) == 0)
			return true;

	return false;
}
