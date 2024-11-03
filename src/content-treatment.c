#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "heads.h"

// to `getSpecial`
// and its process
static char _c;

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
