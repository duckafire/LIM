#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "start.h"
#include "input.h"
#include "dstr.h"

static char c;

bool getName(char c, bool *fr, bool *fc){
	if(IS_CHR(c) || (IS_NUM(c) && !*fc)){
		if(!*fr) *fr = true; // first character (of the file) was writted
		if(!*fc) *fc = true; // it is the first character

		dstr_addc(&gp_curWord, c);
		
		return true;
	}

	return false;
}

bool getNum(char c){
	if(IS_NUM(c)){
		dstr_addc(&gp_curWord, c);
	
		return true;
	}

	return false;
}

bool getSpace(char c, bool fr, bool *lf){
	if(IS_SPC(c)){
		if(fr && !*lf){
			*lf = true;
			dstr_addc(&gp_curWord, '\n');
		}
		return true;
	}

	return false;
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
	dstr_addc(&gp_curWord, c);
}

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
		dstr_addc(&gp_curWord, c);

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
		dstr_addc(&gp_curWord, c);
		dstr_addc(&gp_curWord, c);
		dstr_fputs(&gp_curWord);

		return true;
	}
	fseek(gf_origin, -1, SEEK_CUR);
	
	return false;
}
