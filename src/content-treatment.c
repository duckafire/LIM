#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "heads.h"

// to `ct_getSpecial`
// and its process
static char _c;

static short kw_len;
static char **keywords;
static char lua_kw_x2[5][3]={"do","if","in","or", "_G"};
static char lua_kw_x3[5][4]={"and","end","for","nil","not"};
static char lua_kw_x4[3][5]={"else","then","true"};
static char lua_kw_x5[5][6]={"break","false","local","until","while"};
static char lua_kw_x6[3][7]={"elseif","repeat","return"};
static char lua_kw_x8[9]   ="function";
static char lua_funcs[23][15]  ={"assert","next","require","collectgarbage","pairs","select","dofile","pcall","setmetatable","error","print","tonumber","setmetatable","rawequal","tostring","ipairs","rawget","type","load","rawlen","xpcall","loadfile","rawset"};



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

short ct_checkLuaKeywords(char *word){
	short kw_len = strlen(word);

	if(kw_len < 2 || kw_len > 9)
		return LUA_NONE_KW;

	switch(kw_len){
		case 2:
			for(short i = 0; i < kw_len; i++)
				if(strcmp(word, lua_kw_x2[i]) == 0){
					if(i < 2)
						return LUAB_OPEN;
					return LUA_NOB;
				}
			break;
		case 3:
			for(short i = 0; i < kw_len; i++)
				if(strcmp(word, lua_kw_x3[i]) == 0){
					if(i == 1)
						return LUAB_CLOSE;
					else if(i == 2)
						return LUAB_OPEN;
					return LUA_NOB;
				}
			break;
		case 4:
			for(short i = 0; i < kw_len; i++)
				if(strcmp(word, lua_kw_x4[i]) == 0)
					return LUA_NOB;
			break;
		case 5:
			for(short i = 0; i < kw_len; i++)
				if(strcmp(word, lua_kw_x5[i]) == 0){
					if(i == 4)
						return LUAB_OPEN;
					return LUA_NOB;
				}
			break;
		case 6:
			for(short i = 0; i < kw_len; i++)
				if(strcmp(word, lua_kw_x6[i]) == 0)
					return LUA_NOB;
			break;
		case 8:
			if(strcmp(word, lua_kw_x8) == 0)
				return LUAB_OPEN;
			break;
		default: return 0; break;
	}

	return LUA_NONE_KW;
}

short ct_checkLuaFunc(char *word){
	// ipairs, next, setmetatable, ...
	for(short i = 0; i < 23; i++)
		if(strcmp(word, lua_funcs[i]) == 0)
			return LUA_FUNC;

	return LUA_NONE_KW;
}

short ct_checkPrefixNow(char *word, short last, bool isRootEnv){
	// to library
	if(last == PREFIX_LIB_FUNC || last == PREFIX_LIB_VAR || last == PREFIX_GLOBAL_FUNC || last == PREFIX_LOCAL_FUNC)
		return last;

	// [local] function
	if(strcmp(word, "function") == 0 && (last == PREFIX_GLOBAL || last == PREFIX_LOCAL))
			return last;

	// [local] variable/table
	switch(last){
		case PREFIX_GLOBAL: return PREFIX_GLOBAL_VAR;
		case PREFIX_LOCAL:  return PREFIX_LOCAL_VAR;
	}

	// function(
	if(strcmp(word, "function(") == 0)
		return PREFIX_ANONYMOUS;

	return PREFIX_NONE;
}

short ct_contentType(char *word, short prefix){
	// local function <name>
	if(prefix == PREFIX_GLOBAL || prefix == PREFIX_LOCAL)
		return TYPE_CONSTANT;

	// identifiers
	if(isalpha(word[0]) != 0 || word[0] == '_'){
		// metamethod
		if(word[1] == '_')
			return TYPE_CONSTANT;

		// function
		if(word[strlen(word) - 1] == '('){
			switch(prefix){
				case PREFIX_LIB_FUNC:    return TYPE_LIB_FUNC;
				case PREFIX_GLOBAL_FUNC: return TYPE_GLOBAL_FUNC;
				case PREFIX_LOCAL_FUNC:  return TYPE_LOCAL_FUNC;
				default: return TYPE_USE_OR_CALL;
			}
		}

		// variables and tables
		switch(prefix){
			case PREFIX_LIB_VAR:    return TYPE_LIB_VAR;
			case PREFIX_GLOBAL_VAR: return TYPE_GLOBAL_VAR;
			case PREFIX_LOCAL_VAR:  return TYPE_LOCAL_VAR;
			default: return TYPE_USE_OR_CALL;
		}
	}

	// numbers, table keys and
	// special characteres
	return TYPE_CONSTANT;
}

short ct_checkPrefixNextCycle(char *word, short cur, bool isRootEnv){
	if(strcmp(word, "_G") == 0)
		return PREFIX_LIB_VAR;

	if(strcmp(word, "local") == 0)
		if(isRootEnv)
			return PREFIX_GLOBAL;
		else
			return PREFIX_LOCAL;

	if(strcmp(word, "function") == 0){
		switch(cur){
			case PREFIX_GLOBAL: return PREFIX_GLOBAL_FUNC;
			case PREFIX_LOCAL:  return PREFIX_LOCAL_FUNC;
			default: return PREFIX_LIB_FUNC;
		}
	}

	return PREFIX_NONE;
}
