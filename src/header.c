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

	if(flags.untilStage != 0)
		return HEADER_BLOCK_US;

	// open and check if
	// "head.lim" exist
	FILE *origin;
	origin = fopen("header.lim", "r");
	if(origin == NULL){
		flags.headfile = false;
		return HEADER_NOT_FOUND;
	}

	// "lastTell": position of the file cursor,
	// BEFORE "origin" is readed, MORE ONE;
	// "curTell": position of the file cursor,
	// AFTER "origin" is readed;
	// if "curTell > lastTell" meaning that one,
	// or more, characteres, different of EOF
	// and '@', was/were readed and printed,
	// because this, they will be considered a
	// valid partition.
	long lastTell, curTell = 1;

	header[HEADER_TOP]   = NULL;
	header[HEADER_SCOPE] = NULL;
	header[HEADER_LIST]  = NULL;

	header[HEADER_TOP] = tmpfile();
	if(header_getFromOrigin(origin, header[HEADER_TOP], &lastTell, &curTell)){
		// <content-top>
		// [@]
		if(curTell > lastTell)
			return HEADER_ONLY_TOP;

		// <NONE>
		fclose(header[HEADER_TOP]);
		header[HEADER_TOP] = NULL;
		return HEADER_NONE_PART;
	}
	
	header[HEADER_SCOPE] = tmpfile();
	if(header_getFromOrigin(origin, header[HEADER_SCOPE], &lastTell, &curTell)){
		// <content-top>
		// <@>
		// <content-scope>
		if(curTell > lastTell)
			return HEADER_NO_LIST;

		// <content-top>
		// [@]
		fclose(header[HEADER_SCOPE]);
		header[HEADER_SCOPE] = NULL;
		return HEADER_ONLY_TOP;
	}

	header[HEADER_LIST] = tmpfile();
	header_getFromOrigin(origin, header[HEADER_LIST], &lastTell, &curTell);

	// <content-top>
	// <@>
	// <content-scope>
	// <@>
	// <content-list>
	if(curTell > lastTell)
		return HEADER_SUCCESS;

	// <content-top>
	// <@>
	// <content-scope>
	// [@]
	fclose(header[HEADER_LIST]);
	header[HEADER_LIST] = NULL;
	return HEADER_NO_LIST;

	// BUG: about "header.lim"
	// if it have: @ [\n]
	// HEADER_ONLY_TOP will be returned
	// if it have: @ [\n] @[\n]
	// HEADER_NO_LIST will be returned
	// if it have: @ [\n] @ [\n] @ [\n | @] [...]
	// HEADER_SUCCESS will be returned
}

static bool header_getFromOrigin(FILE *src, FILE *dest, long *ltell, long *ctell){
	bool space = false;
	bool lfeed = true;

	// update LAST tell
	*ltell = *ctell + 1;

	while((c = fgetc(src)) != EOF){
		// a new partition was found;
		// they need to be placed
		// in line start (^@)
		if(c == '@' && lfeed){
			*ctell = ftell(src);

			// a line feed is expected after
			// of '@' (it will be jumped),
			// but it is not mandatory
			if(fgetc(src) != '\n')
				fseek(src, -1, SEEK_CUR);

			return false; // != EOF
		}

		// line feed and tabulation
		// are not allow
		lfeed = (c == '\n');

		if(lfeed || c == '\t'){
			if(space)
				continue;

			fputc(' ', dest);
			space = true;
			continue;
		}

		// sequences of spaces
		// are not allow
		if(c == ' '){
			if(space)
				continue;
			else
				space = true;
		}else{
			space = false;
		}
		
		// print content
		fputc(c, dest);
	}

	// update CURRENT tell
	*ctell = ftell(src);

	// c == EOF
	fclose(src);
	return true;
}

bool header_printTop(FILE *dest){
	if(!flags.headfile || header[HEADER_TOP] == NULL)
		return false;

	t_fcat(header[HEADER_TOP], dest);
	fputc('\n', dest);
	return true;
}

bool header_printScope(FILE *dest){
	if(!flags.headfile || header[HEADER_SCOPE] == NULL)
		return false;

	t_fcat(header[HEADER_SCOPE], dest);
	return true;
}

bool header_checkFuncList(char *word){
	if(!flags.headfile || header[HEADER_LIST] == NULL)
		return false;

	char c;

	// for "end" string before
	// to leave this function
	bool found = false;

	mm_stringInit(&string);
	fseek(header[HEADER_LIST], 0, SEEK_SET);

	while((c = fgetc(header[HEADER_LIST])) != EOF){
		if(c != ' '){
			mm_stringAdd(&string, c);
			continue;
		}

		if(strcmp(word, string) == 0){
			found = true;
			break;
		}

		mm_stringEnd(&string, true);
	}

	mm_stringEnd(&string, false);
	return found;
}

FILE* header_getList(void){
	return header[HEADER_LIST];
}

void header_end(){
	if(!flags.headfile || header[HEADER_TOP] == NULL)
		return;

	fclose(header[HEADER_TOP]);
	header[HEADER_TOP] = NULL;

	if(header[HEADER_SCOPE] != NULL){
		fclose(header[HEADER_SCOPE]);
		header[HEADER_SCOPE] = NULL;
	}

	if(header[HEADER_LIST] != NULL){
		fclose(header[HEADER_LIST]);
		header[HEADER_LIST] = NULL;
	}

	if(string != NULL)
		mm_stringEnd(&string, false);
}
