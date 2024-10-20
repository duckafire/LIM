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

	if(c == '\'' || c == '"'){
		saveString(c);
		return;
	}
}

static char c;

static void clearComment(bool isLine){
	if(isLine){
		fseek(gf_origin, -2, SEEK_CUR); // remove '--'
		
		while((c = fgetc(gf_origin)) != EOF && c != '\n');
		return;
	}

	fseek(gf_origin, -4, SEEK_CUR); // remove '--[['

	while((c = fgetc(gf_origin)) != EOF)
		if(c == ']')
			if(fgetc(gf_origin) == ']')
				break;
}

static void saveString(char signal){
	bool invBar = false; // '\'

	do{
		dstr_addc(c);

		if(invBar){
			invBar = false;
			continue;
		}

		if(c == '\\'){
			invBar = true;
			continue;
		}
	}while((c = fgetc(gf_origin)) != EOF && c != signal);

	dstr_addc(c);
	dstr_addc('\n');
}

static bool saveDoubleSignal(char signal){
	if(c == signal){
		if(fgetc(gf_origin) == signal){
			dstr_addc(c);
			dstr_addc(c);

			return true;
		}

		fseek(gf_origin, -1, SEEK_CUR);
	}
	
	return false;
}
