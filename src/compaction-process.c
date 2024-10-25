#include <stddef.h>
#include <stdbool.h>
#include "heads.h"

static char c;

void cp_0_checkAndOpenFiles(void){
	info_verbose();

	gf_origin = fopen(gp_nameOrg, "r");
	if(gf_origin == NULL)
		nonExistentFile(gp_nameOrg);

	if(!g_verbose){
		FILE *dst;
		dst = fopen(gp_nameDst, "r");

		if(dst != NULL){
			fclose(dst);
			fileAlreadyExistent(gp_nameDst);
		}
	}

	collect_init();
}

void cp_1_extractionFromOrigin(void){
	info_verbose();

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
				collect_add('\n');

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
					collect_add('.');

					c = fgetc(gf_origin);
					if(getNum(c)){
						isFloat = true;
						continue;
					}

					fseek(gf_origin, -1, SEEK_CUR);
					collect_add('\n');
					continue;
				}

				fseek(gf_origin, -1, SEEK_CUR);
			}

			continue;
		}

		if(firstChar && !dotGetted){
			firstChar = dotGetted = false;
			collect_add('\n');
		}

		if(c == '.' && isFloat)
			collect_add('\n');

		getSpecial(c);
		isSpecial = dotExpected = true;
		isFloat = false;
	}
}

void cp_x_tempFinish(void){
	info_verbose();
	collect_end();
	info_verbose();
}
