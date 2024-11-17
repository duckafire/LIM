#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "heads.h"

static char c;
static HeaderFile head;

char* head_init(void){
	if(!g_headfile)
		return HEADER_BLOCKED;

	// open and check if
	// "head.lim" exist
	FILE *origin;
	origin = fopen("header.lim", "r");
	if(origin == NULL){
		g_headfile = false;
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

	head.top   = NULL;
	head.scope = NULL;
	head.list  = NULL;

	head.top = tmpfile();
	if(head_getFromOrigin(origin, head.top, &lastTell, &curTell)){
		// <content-top>
		// [@]
		if(curTell > lastTell)
			return HEADER_ONLY_TOP;

		// <NONE>
		fclose(head.top);
		head.top = NULL;
		return HEADER_NONE_PART;
	}
	
	head.scope = tmpfile();
	if(head_getFromOrigin(origin, head.scope, &lastTell, &curTell)){
		// <content-top>
		// <@>
		// <content-scope>
		if(curTell > lastTell)
			return HEADER_NO_LIST;

		// <content-top>
		// [@]
		fclose(head.scope);
		head.scope = NULL;
		return HEADER_ONLY_TOP;
	}

	head.list = tmpfile();
	head_getFromOrigin(origin, head.list, &lastTell, &curTell);

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
	fclose(head.list);
	head.list = NULL;
	return HEADER_NO_LIST;

	// BUG: about "header.lim"
	// if it have: @ [\n]
	// HEADER_ONLY_TOP will be returned
	// if it have: @ [\n] @[\n]
	// HEADER_NO_LIST will be returned
	// if it have: @ [\n] @ [\n] @ [\n | @] [...]
	// HEADER_SUCCESS will be returned
}

static bool head_getFromOrigin(FILE *src, FILE *dest, long *ltell, long *ctell){
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

bool head_printTop(FILE *dest){
	if(!g_headfile || head.top == NULL)
		return false;

	tools_fcat(head.top, dest);
	fputc('\n', dest);
	return true;
}

bool head_printScope(FILE *dest){
	if(!g_headfile || head.scope == NULL)
		return false;

	tools_fcat(head.scope, dest);
	return true;
}

bool head_checkFuncList(char *word){
	if(!g_headfile || head.list == NULL)
		return false;

	char c;

	// for "end" head.word before
	// to leave this function
	bool found = false;

	head_initWord();
	fseek(head.list, 0, SEEK_SET);

	while((c = fgetc(head.list)) != EOF){
		if(c != ' '){
			head_addWord(c);
			continue;
		}

		if(strcmp(word, head.word) == 0){
			found = true;
			break;
		}

		head_endWord(true);
	}

	head_endWord(false);
	return found;
}

void head_initWord(void){
	tools_initDinStr(&head.word);
}

void head_addWord(char c){
	tools_addDinStr(head.word, c);
}

char *head_getWord(void){
	return head.word;
}

void head_endWord(bool restart){
	tools_endDinStr(&head.word, restart);
}

FILE *head_getList(void){
	return head.list;
}

void head_end(){
	if(!g_headfile || head.top == NULL)
		return;

	fclose(tools_copyFile(head.list, "list.lim"));

	fclose(head.top);
	head.top = NULL;

	if(head.scope != NULL){
		fclose(head.scope);
		head.scope = NULL;
	}

	if(head.list != NULL){
		fclose(head.list);
		head.list = NULL;
	}

	if(head.word != NULL)
		head_endWord(false);
}
