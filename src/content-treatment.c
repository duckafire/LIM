#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "heads.h"

// to `getSpecial`
// and its process
static char _c;

static short kw_len;
static char **keywords;
static char lua_kw_x2[5][3]={"do","if","in","or", "_G"};
static char lua_kw_x3[5][4]={"and","end","for","nil","not"};
static char lua_kw_x4[3][5]={"else","then","true"};
static char lua_kw_x5[5][6]={"break","false","local","until","while"};
static char lua_kw_x6[2][7]={"elseif","repeat"};
static char lua_kw_x9[9] = "function";

bool getIdentifier(char *c, bool isFirst){
	if(*c == ' ')
		*c = clearSpaces();

	return ((isalpha(*c) != 0 || *c == '_') || (!isFirst && (isdigit(*c) || *c == '.' || *c == '(')));
}

char clearSpaces(void){
	char c;
	while(FGETC == ' ' && c != EOF);
	return c;
}

void getSpecial(char c){
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
		saveString(c);
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

static void saveString(char signal){
	bool invBar = false; // '\'

	do{
		collect_add(_c);

		if(invBar){
			invBar = false;
			continue;
		}

		if(_c == '\\'){
			invBar = true;
			continue;
		}
	}while((_c = fgetc(gf_origin)) != EOF && _c != signal);

	collect_add(_c);
	collect_add('\n');
}

static void saveBraces(void){
	unsigned int qtt = 1;

	while(qtt > 0){
		if(!isgraph(_c))
			collect_add(_c);

		_c = fgetc(gf_origin);

		if(_c == '{')
			qtt++;
		else if(_c == '}')
			qtt--;
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

short contentType(char *word, short prefix){
	if(checkLuaKeywords(word))
		return TYPE_CONSTANT;

	if(isalpha(word[0]) != 0 || word[0] == '_'){
		// function
		if(word[strlen(word) - 1] == '('){
			if(prefix == PREFIX_LIB_FUNC)
				return TYPE_LIB_FUNC;

			if(prefix == PREFIX_GLOBAL_FUNC)
				return TYPE_GLOBAL_FUNC;

			return TYPE_LOCAL_FUNC;
		}

		// metamethod
		if(word[1] == '_')
			return TYPE_CONSTANT;

		// variables and tables
		if(prefix == PREFIX_LIB_VAR)
			return TYPE_LIB_VAR;

		if(prefix == PREFIX_GLOBAL_VAR)
			return TYPE_GLOBAL_VAR;

		return TYPE_LOCAL_VAR;
	}

	// numbers, table keys and
	// special characteres
	return TYPE_CONSTANT;
}

short checkPrefixNow(char *word, short last){
	// function(
	if(strcmp(word, "function(") == 0)
		return PREFIX_ANONYMOUS;

	// local function
	if((last == PREFIX_GLOBAL || last == PREFIX_LOCAL) && strcmp(word, "function") == 0)
		if(last == PREFIX_GLOBAL)
			return PREFIX_GLOBAL_FUNC;
		else
			return PREFIX_LOCAL_FUNC;

	if(last == PREFIX_GLOBAL)
		return PREFIX_GLOBAL_VAR;

	if(last == PREFIX_LOCAL)
		return PREFIX_LOCAL_VAR;

	return PREFIX_NONE;
}

short checkPrefixNextCycle(char *word, bool isRootEnv){
	if(strcmp(word, "_G"))
		return PREFIX_LIB_VAR;

	if(strcmp(word, "local"))
		if(isRootEnv)
			return PREFIX_GLOBAL;
		else
			return PREFIX_LOCAL;

	if(strcmp(word, "function"))
		return PREFIX_LIB_FUNC;

	return PREFIX_NONE;
}

static short checkLuaKeywords(char *word){
	short kw_len = strlen(word);

	if(kw_len < 2 || kw_len > 9)
		return 0;

	switch(kw_len){
		case 2:
			for(short i = 0; i < kw_len; i++)
				if(strcmp(word, lua_kw_x2[i]) == 0)
					return 1;
			break;
		case 3:
			for(short i = 0; i < kw_len; i++)
				if(strcmp(word, lua_kw_x3[i]) == 0)
					return 1;
			break;
		case 4:
			for(short i = 0; i < kw_len; i++)
				if(strcmp(word, lua_kw_x4[i]) == 0)
					return 1;
			break;
		case 5:
			for(short i = 0; i < kw_len; i++)
				if(strcmp(word, lua_kw_x5[i]) == 0)
					return 1;
			break;
		case 6:
			for(short i = 0; i < kw_len; i++)
				if(strcmp(word, lua_kw_x6[i]) == 0)
					return 1;
			break;
		case 9:
			if(strcmp(word, lua_kw_x9) == 0)
				return 1;
			break;
		default: return 0; break;
	}

	return 0;
}
