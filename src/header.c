#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "heads.h"

static char c;
static FILE *header[3];
static char *string;

char* header_init(void){
	if(!flags.headfile)
		return HEADER_BLOCK_NH;


	FILE *origin;
	origin = fopen("header.lim", "r");
	if(origin == NULL){
		flags.headfile = false;
		return HEADER_NOT_FOUND;
	}


	long lastTell, curTell = 1;

	header[HEADER_TOP]   = NULL;
	header[HEADER_SCOPE] = NULL;
	header[HEADER_LIST]  = NULL;


	header[HEADER_TOP] = tmpfile();
	if(header_getFromOrigin(origin, HEADER_TOP, &lastTell, &curTell)){
		// <content-top> ; [@]
		if(curTell > lastTell)
			return HEADER_ONLY_TOP;

		// <NONE>
		fclose(header[HEADER_TOP]);
		header[HEADER_TOP] = NULL;
		return HEADER_NONE_PART;
	}
	

	header[HEADER_SCOPE] = tmpfile();
	if(header_getFromOrigin(origin, HEADER_SCOPE, &lastTell, &curTell)){
		// <content-top> ; <@> ; <content-scope>
		if(curTell > lastTell)
			return HEADER_NO_LIST;

		// <content-top> ; [@]
		fclose(header[HEADER_SCOPE]);
		header[HEADER_SCOPE] = NULL;
		return HEADER_ONLY_TOP;
	}


	header[HEADER_LIST] = tmpfile();
	header_getFromOrigin(origin, HEADER_LIST, &lastTell, &curTell);

	// <content-top> ; <@> ; <content-scope> ; <@> ; <content-list>
	if(curTell > lastTell)
		return HEADER_SUCCESS;

	// <content-top> ; <@> ; <content-scope> ; [@]
	fclose(header[HEADER_LIST]);
	header[HEADER_LIST] = NULL;
	return HEADER_NO_LIST;
}

static bool header_getFromOrigin(FILE *src, short buf, long *ltell, long *ctell){
	bool space = false;
	bool lfeed = true;

	// update LAST tell
	*ltell = *ctell + 1;

	while((c = fgetc(src)) != EOF){
		// a new partition was found (<^@>[\n])
		if(c == '@' && lfeed){
			*ctell = ftell(src);

			if(fgetc(src) != '\n')
				fseek(src, -1, SEEK_CUR);

			// end of partition (or EOF)
			return false;
		}

		if(buf != HEADER_TOP){
			// jump line feed and tabulations
			lfeed = (c == '\n');

			if(lfeed || c == '\t'){
				if(space)
					continue;

				fputc(((buf == HEADER_SCOPE) ? ' ' : '\n'), header[buf]);
				space = true;
				continue;
			}

			// jump sequences of spaces
			if(c == ' '){
				if(space)
					continue;
				else
					space = true;
			}else{
				space = false;
			}
		}
		
		// valid
		fputc(c, header[buf]);
	}

	// update CURRENT tell
	*ctell = ftell(src);


	// c == EOF
	fclose(src);
	return true;
}

bool header_printBuf(FILE *dest, short buf){
	if(!flags.headfile || header[buf] == NULL)
		return false;

	t_fcat(header[buf], dest);
	return true;
}

bool header_checkFuncList(char *word){
	if(!flags.headfile || header[HEADER_LIST] == NULL)
		return false;

	char c;

	mm_stringInit(&string);
	fseek(header[HEADER_LIST], 0, SEEK_SET);

	while((c = fgetc(header[HEADER_LIST])) != EOF){
		t_getStringFromFile(header[HEADER_LIST], &c, &string);

		if(strcmp(word, string) == 0){
			mm_stringEnd(&string, false);
			return true;
		}

		mm_stringEnd(&string, true);
	}

	mm_stringEnd(&string, false);
	return false;
}

FILE* header_getFuncList(void){
	return header[HEADER_LIST];
}

void header_end(){
	if(!flags.headfile)
		return;
	
	for(short i = 0; i < 3; i++){
		if(header[i] != NULL){
			fclose(header[i]);
			header[i] = NULL;
		}
	}

	if(string != NULL)
		mm_stringEnd(&string, false);
}
