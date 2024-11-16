#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "heads.h"

static char c;
static HeaderFile head;

void head_init(void){
	if(!g_headfile)
		return;

	FILE *origin;
	origin = fopen("header.lim", "r");
	if(origin == NULL){
		g_headfile = false;
		return;
	}

	head.top   = NULL;
	head.scope = NULL;
	head.list  = NULL;

	head.top = tmpfile();
	if(head_getFromOrigin(origin, head.top))
		return;
	
	head.scope = tmpfile();
	if(head_getFromOrigin(origin, head.top))
		return;

	head.list = tmpfile();
	head_getFromOrigin(origin, head.top);

	fclose(origin);
}

static bool head_getFromOrigin(FILE *src, FILE *dest){
	bool isSpace = false;

	while((c = fgetc(src)) != '@' && c != EOF){
		if(c == ' '){
			if(isSpace)
				continue;
			else
				isSpace = true;
		}else{
			isSpace = false;
		}

		if(c == '\t' || c == '\n'){
			if(isSpace)
				continue;

			fputc(' ', dest);
			continue;
		}

		fputc(c, dest);
	}

	return (c == EOF);
}

void head_printTop(FILE *dest){
	if(!g_headfile || head.top == NULL)
		return;

	tools_fcat(head.top, dest);
}

void head_printScope(FILE *dest){
	if(!g_headfile || head.scope == NULL)
		return;

	tools_fcat(head.scope, dest);
}

bool head_checkFuncList(char *word){
	if(!g_headfile || head.list == NULL)
		return false;

	char c;

	fseek(head.list, 0, SEEK_SET);

	while((c = fgetc(head.list)) != EOF){
		if(c != ' '){
			head_addWord(c);
			continue;
		}

		if(strcmp(word, head.word) == 0)
			return true;
	}

	return false;
}

static void head_initWord(void){
	tools_initDimStr(&head.word);
}

static void head_addWord(char c){
	tools_addDimStr(head.word, c);
}

static void head_endWord(bool restart){
	tools_endDimStr(&head.word, restart);
}

void head_end(){
	if(!g_headfile)
		return;

	fclose(head.top);
	fclose(head.scope);
	fclose(head.list);

	head.top   = NULL;
	head.scope = NULL;
	head.list  = NULL;

	if(head.word != NULL)
		head_endWord(false);
}
