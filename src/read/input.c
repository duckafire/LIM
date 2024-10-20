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

	// 1.1..1...
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

void getSpecial(char c){
	// --, --[[, ', ", .., //
	if(c == '-' && fgetc(gf_origin) == '-'){
		if(fgetc(gf_origin) == '[' && fgetc(gf_origin) == '[')
			clearComment(true);

		fseek(gf_origin, -2, SEEK_CUR);
		clearComment(false);
		return;
	}

	if(c == '\'' || c == '"'){
		saveString(c);
		return;
	}

	if(saveDoubleSignal('.')) return;
	if(saveDoubleSignal('/')) return;
	dstr_addc(c);
}

static char c;

static void clearComment(bool isBlock){
	if(!isBlock){
		//fseek(gf_origin, -5, SEEK_CUR); // remove '-'
		
		while((c = fgetc(gf_origin)) != EOF && c != '\n');
		return;
	}

	//fseek(gf_origin, -3, SEEK_CUR); // remove '-'

	while((c = fgetc(gf_origin)) != EOF)
		if(c == ']')
			if(fgetc(gf_origin) == ']')
				break;
}

static void saveString(char signal){
	bool invBar = false; // '\'

	while((c = fgetc(gf_origin)) != EOF){
		dstr_addc(c);

		if(invBar){
			invBar = false;
			continue;
		}

		if(c == '\\'){
			invBar = true;
			continue;
		}

		if(c == signal) break;
	}
}

static bool saveDoubleSignal(char signal){
	if(c == signal && fgetc(gf_origin) == signal){
		dstr_addc(c);
		dstr_addc(c);
		dstr_fputs();

		return true;
	}
	fseek(gf_origin, -1, SEEK_CUR);
	
	return false;
}
