#include <stdio.h>
#include <stdbool.h>

#include "../head.h"
#include "../messages/head.h"
#include "head.h"

FILE *gf_origin  = NULL;
FILE *gf_buffer  = NULL;

static char c;

void startProcess(void){
	verbose();

	gf_origin = fopen(gp_nameOrg, "r");
	if(gf_origin == NULL)
		nonExistentFile(gp_nameOrg);

	gf_buffer = tmpfile();

	if(!g_replace && fopen(gp_nameDst, "r") != NULL)
		fileAlreadyExistent(gp_nameDst);

	dstr_init();
	getContent();

	// temp
	verbose();
	dstr_fputs();
	dstr_end();
	verbose();
}

static void getContent(void){
	verbose();

	void turnOn(bool *b){
		if(!*b)
			*b = true;
	}

	// it is the first character
	bool firstChar = false;

	// line feed
	bool lineFeed = false;

	// the fisrt character was writed
	bool firstOfFile = false;
	
	// a special character was finded and it prints '\n'
	bool isSpecial = false;

	// to line feed after float numbers
	bool isFloat = false;

	// a number get a dot to itself
	bool dotGetted = false;

	// to float numbers
	bool dotExpected = true;

	while((c = fgetc(gf_origin)) != EOF){
		if(getSpace(c)){
			if(isSpecial){
				lineFeed = true;
				isSpecial = isFloat = dotGetted = false;
				continue;
			}

			if((firstOfFile || firstChar) && !lineFeed){
				dstr_addc('\n');

				lineFeed = true;
				firstChar = isSpecial = isFloat = dotGetted = false;
			}
			continue;
		}

		if(getName(c, firstChar)){
			turnOn(&firstOfFile);
			turnOn(&firstChar);

			lineFeed = isSpecial = dotGetted = false;
			continue;
		}

		if(getNum(c)){
			firstChar = true;
			lineFeed = isSpecial = false;

			if(dotExpected){
				if(fgetc(gf_origin) == '.'){
					dotExpected = false;
					dotGetted = true;
					dstr_addc('.');

					c = fgetc(gf_origin);
					if(getNum(c)){
						isFloat = true;
						continue;
					}

					fseek(gf_origin, -1, SEEK_CUR);
					dstr_addc('\n');
					continue;
				}

				fseek(gf_origin, -1, SEEK_CUR);
			}

			continue;
		}

		if(firstChar && !dotGetted){
			firstChar = dotGetted = false;
			dstr_addc('\n');
		}

		if(c == '.' && isFloat)
			dstr_addc('\n');

		getSpecial(c);
		isSpecial = dotExpected = true;
		isFloat = false;
	}
}
