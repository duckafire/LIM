#include <stdio.h>
#include <stdbool.h>

#include "../head.h"
#include "../messages/head.h"
#include "head.h"

FILE *gf_origin  = NULL;
FILE *gf_buffer  = NULL;

static char c;

void startProcess(void){
	gf_origin = fopen(gp_nameOrg, "r");
	if(gf_origin == NULL)
		nonExistentFile(gp_nameOrg);

	gf_buffer = tmpfile();

	if(!g_replace && fopen(gp_nameDst, "r") != NULL)
		fileAlreadyExistent(gp_nameDst);

	dstr_init();
	getContent();
}

static void getContent(void){
	void turnOn(bool *b){
		if(!*b)
			*b = true;
	}

	// it is the first character
	bool firstOfWord = false;

	// line feed
	bool lineFeed = false;

	// the fisrt character (of the file) was writted
	bool firstOfFile = false;

	// if "it == false" the current string is a identificator
	bool isNumber = false;

	while((c = fgetc(gf_origin)) != EOF){
		if(getSpace(c)){
			if(firstOfFile && !lineFeed){
				lineFeed = true;
				dstr_addc('\n');

				firstOfWord = isNumber = false;
			}
			continue;
		}

		if(getName(c, firstOfWord)){
			turnOn(&firstOfFile);
			turnOn(&firstOfWord);

			lineFeed = false;
			isNumber = false;
			continue;
		}

		firstOfWord = lineFeed = false;

		if(getNum(c, isNumber)){
			turnOn(&isNumber);
			continue;
		}

		dstr_addc(c);
		//getSpecial(c);
	}

	dstr_fputs();
	dstr_end();
}
