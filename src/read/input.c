#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "head.h"

bool getName(char c, bool firstOfWord){
	if(IS_CHR(c) || (IS_NUM(c) && firstOfWord)){
		dstr_addc(c);
		return true;
	}
	return false;
}

bool getNum(char c, bool isFloat){
	static bool isDot = false;

	if(IS_NUM(c) || (c == '.' && isFloat)){
		if(c == '.'){
			if(isDot){
				isDot = false;
				return false;
			}
			isDot = !isDot;
		}

		dstr_addc(c);
		
		return true;
	}
	return false;
}

bool getSpace(char c){
	return (IS_BIN(c));
}

static char _c;

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

	_c = c;
	if(c == '\'' || c == '"'){
		saveString(c);
		return;
	}

	if(c == '{'){
		saveBraces();
		return;
	}

	if(saveDoubleSignal('=')) return;
	if(saveDoubleSignal('/')) return;
	if(saveDoubleSignal('.')) return;

	dstr_addc(c);
	dstr_addc('\n');
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
		dstr_addc(_c);

		if(invBar){
			invBar = false;
			continue;
		}

		if(_c == '\\'){
			invBar = true;
			continue;
		}
	}while((_c = fgetc(gf_origin)) != EOF && _c != signal);

	dstr_addc(_c);
	dstr_addc('\n');
}

static void saveBraces(void){
	UNINT qtt = 1;

	while(qtt > 0){
		if(!IS_BIN(_c))
			dstr_addc(_c);

		_c = fgetc(gf_origin);

		if(_c == '{')
			qtt++;
		else if(_c == '}')
			qtt--;
	}

	dstr_addc('}');
	dstr_addc('\n');
}

static bool saveDoubleSignal(char signal){
	if(_c == signal){
		if(fgetc(gf_origin) == signal){
			dstr_addc(_c);
			dstr_addc(_c);
			dstr_addc('\n');

			return true;
		}

		fseek(gf_origin, -1, SEEK_CUR);
	}
	
	return false;
}
