#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "heads.h"

static FILE *collect = NULL;
static char *ident = NULL; // identifier
static GlobalEnv global;

void buffers_atexit(void){
	if(collect != NULL)
		collect_end();

	if(ident != NULL)
		ident_end(0);

	if(global.head != NULL)
		global_end();
}




////////// COLLECT //////////

void collect_init(void){
	collect = tmpfile();
}

void collect_add(char c){
	fwrite(&c, sizeof(char), 1, collect);
}

FILE* collect_get(void){
	return collect;
}

void collect_end(void){
	fclose(collect);
	collect = NULL;
}



////////// IDENTIFIER //////////

void ident_init(void){
	ident = malloc(1);
	ident[0] = '\0';
}

void ident_add(char c){
	unsigned int len = strlen(ident) + 3;

	char *temp;
	temp = malloc(len);

	memset(temp, '\0', len);
	strcpy(temp, ident);
	temp[len - 3] = c;

	free(ident);
	ident = temp;
}

char* ident_get(void){
	return ident;
}

void ident_end(short restart){
	free(ident);
	ident = NULL;

	if(restart)
		ident_init();
}



////////// GLOBAL ENVORINMENTS //////////

void global_init(void){
	// used one time
	global.order     = tmpfile();
	global.libVar    = tmpfile();
	global.libFunc   = tmpfile();
	global.var       = tmpfile();
	global.func      = tmpfile();
	global.useOrCall = tmpfile();
	global.constants = tmpfile();
	global.luaFunc   = tmpfile();
	
	global.head = NULL;
	global.tail = NULL;
}

void global_newEnv(char *name){
	// insert in chain end
	FuncEnv *new;
	new = malloc(sizeof(FuncEnv));
	new->name = malloc(sizeof(name));
	new->func = tmpfile();
	new->var  = tmpfile();
	new->next = NULL;

	// the '(' is included
	strcpy(new->name, name);

	if(global.head == NULL){
		global.head = new;
		global.tail = new;
		return;
	}

	global.tail->next = new;
	global.tail = new;
}

void global_order(short code, char *word){
	fwrite(&code, sizeof(int), 1, global.order);

	fprintf(stdout, "%d - %s\n", code, word);
}

void global_print(char *word, char *name, short bufId){
	// `name == NULL` for print
	// in a global buffer
	FILE *buf;
	unsigned int i = 0;
	char *c = NULL;
	char n = '\n';

	buf = global_getBuf(bufId, name);

	for(c = &word[i]; *c != '\0'; c = &word[++i])
		fwrite(c, sizeof(char), 1, buf);

	fwrite(&n, sizeof(char), 1, buf);
}

void global_rmvEnv(void){
	// remove the last
	if(global.head == global.tail){
		free(global.head);
		global.head = NULL;
		global.tail = NULL;
		return;
	}

	// get element from chain
	FuncEnv *p;
	for(p = global.head; p->next->next != NULL; p = p->next);

	fclose(p->next->func);
	fclose(p->next->var);
	free(p->next);

	p->next = NULL;
	global.tail = p;
}

GlobalEnv* global_get(void){
	return &global;
}

void global_end(void){
	while(global.head != NULL)
		global_rmvEnv();

	fclose(global.order);
	fclose(global.libVar);
	fclose(global.libFunc);
	fclose(global.var);
	fclose(global.func);
	fclose(global.useOrCall);
	fclose(global.constants);
	fclose(global.luaFunc);

	global.order     = NULL;
	global.libVar    = NULL;
	global.libFunc   = NULL;
	global.var       = NULL;
	global.func      = NULL;
	global.useOrCall = NULL;
	global.constants = NULL;
	global.luaFunc   = NULL;
	
	global.head = NULL;
	global.tail = NULL;
}

static FILE* global_getBuf(short bufId, char *name){
	// global
	switch(bufId){
		case TYPE_CONSTANT:    return global.constants; break;
		case TYPE_ANONYMOUS:   return global.constants; break;
		case TYPE_USE_OR_CALL: return global.useOrCall; break;
		case TYPE_FROM_LUA:    return global.luaFunc;   break;
		case TYPE_LIB_FUNC:    return global.libFunc;   break;
		case TYPE_LIB_VAR:     return global.libVar;    break;
		case TYPE_GLOBAL_FUNC: return global.func;      break;
		case TYPE_GLOBAL_VAR:  return global.var;       break;
	}

	// local
	FuncEnv *f;

	if(global.head == global.tail)
		f = global.head;
	else
		for(f = global.head; strcmp(f->name, name) != 0 && f != NULL; f = f->next);

	switch(bufId){
		case TYPE_LOCAL_VAR:  return f->var; break;
		case TYPE_LOCAL_FUNC: return f->func; break;
	}
}
