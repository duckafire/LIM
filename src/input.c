#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "heads.h"

static char _c;

bool getName(char c, bool firstChar){
	if(IS_CHR(c) || (IS_NUM(c) && firstChar)){
		collect_add(c);
		return true;
	}
	return false;
}

bool getNum(char c){
	if(IS_NUM(c)){
		collect_add(c);

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
	UNINT qtt = 1;

	while(qtt > 0){
		if(!IS_BIN(_c))
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

static bool saveDoubleSignal(char signal){
	if(_c == signal){
		if(fgetc(gf_origin) == signal){
			collect_add(_c);
			collect_add(_c);
			collect_add('\n');

			return true;
		}

		fseek(gf_origin, -1, SEEK_CUR);
	}
	
	return false;
}
