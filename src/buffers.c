#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "heads.h"

static FILE *collect;
static char *ident; // identifier
static GlobalEnv global;

// COLLECT
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
}

// IDENTIFIER
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

// GLOBAL ENVORINMENTS
void global_init(void){
	global.order = tmpfile();
	global.libVar = tmpfile();
	global.libFunc = tmpfile();
	global.var = tmpfile();
	global.func = tmpfile();
	global.constants = tmpfile();
	
	global.head = NULL;
	global.tail = NULL;
}

void global_newEnv(void){
	struct LocalEnv *new;
	new = malloc(sizeof(struct LocalEnv));

	new->var = tmpfile();
	new->func = tmpfile();

	if(global.head == NULL)
		global.head = new;

	global.tail->next = new;
	global.tail = new;
}

short global_print(char *ident, short bufId){
	FILE *buf;
	buf = global_getBuf(bufId);

	unsigned short i = 0;
	char *c = NULL;
	for(c = &ident[0]; *c != '\0'; c = &ident[++i])
		fwrite(c, sizeof(char), 1, buf);

	char n = '\n';
	fwrite(&n, sizeof(char), 1, buf);
}

void global_rmvEnv(void){
	struct LocalEnv *env;

	if(global.head == global.tail){
		free(global.head);
		global.head = NULL;
		global.tail = NULL;
		return;
	}

	for(env = global.head; env->next != global.tail; env = env->next);

	fclose(env->next->var);
	fclose(env->next->func);

	free(env->next);
	env->next = NULL;
	global.tail = env;
}

void global_end(void){
	// this function consider that all
	// local environments were freed
	fclose(global.order);
	fclose(global.libVar);
	fclose(global.libFunc);
	fclose(global.var);
	fclose(global.func);
	fclose(global.constants);
	
	free(global.head);
	free(global.tail);
}

static FILE* global_getBuf(short bufId){
	switch(bufId){
		case ENV_LIB_VAR:     return global.libVar; break;
		case ENV_LIB_FUNC:    return global.libFunc; break;
		case ENV_GLOBAL_VAR:  return global.var; break;
		case ENV_GLOBAL_FUNC: return global.func; break;
		case ENV_LOCAL_VAR:   return global.tail->var; break;
		case ENV_LOCAL_FUNC:  return global.tail->func; break;
		case ENV_CONSTANTS:   return global.constants; break;
	}
}
