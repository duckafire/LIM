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
	collect = NULL;
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

/* explanation

global   ->   func0   ->   func1   ->   func2   ->   funcn   <-   new func are placed
 |              |            |            |            |          and removed here
env            envn         envn         envn         envn <- new (f)env are placed
                |            |            |            |      and removed here
               env2         env2         env2         env2
                |            |            |            |
               env1         env1         env1         env1
                |            |            |            |
            env0(main)   env0(main)   env0(main)   env0(main)

* func: some function declared in
root of the library environment.

* main: function environment in that
will be stored the identifier declared
in its environment root;
*/

void global_init(void){
	// used one time
	global.order = tmpfile();
	global.libVar = tmpfile();
	global.libFunc = tmpfile();
	global.var = tmpfile();
	global.func = tmpfile();
	global.constants = tmpfile();
	
	global.head = NULL;
	global.tail = NULL;
}

void global_newFuncEnv(char *name){
	// insert in chain end
	FuncEnv *new;
	new = malloc(sizeof(FuncEnv));
	new->name = malloc(sizeof(name));
	new->subEnv = NULL;
	new->next = NULL;

	// the '(' is included
	strcpy(new->name, name);

	if(global.head == NULL){
		global.head = new;
		global.tail = new;
	}else{
		global.tail->next = new;
		global.tail = new;
	}

	// "main" environment
	global_newLocalEnv(name);
}

void global_newLocalEnv(char *name){
	// insert in chain first
	// and push the other
	FuncEnv *func;
	func = global_getLocalEnv(name);

	LocalEnv *new;
	new = malloc(sizeof(LocalEnv));
	new->func = tmpfile();
	new->var = tmpfile();

	if(func->subEnv != NULL)
		new->next = func->subEnv->next;
	else
		new->next = NULL;

	func->subEnv = new;
}

void global_print(char *word, char *name, short bufId){
	// `name == NULL` for print
	// in a global buffer
	FILE *buf;
	buf = global_getBuf(bufId, name);

	unsigned int i = 0;
	char *c = NULL;
	for(c = &word[i]; *c != '\0'; c = &word[i++])
		fwrite(c, sizeof(char), 1, buf);
}

void global_rmvFuncEnv(void){
	// remove the last
	if(global.head == global.tail){
		free(global.head);
		global.head = NULL;
		global.tail = NULL;
		return;
	}

	FuncEnv *p;

	for(p = global.head; p->next->next != NULL; p = p->next);

	free(p->next);
	p->next = NULL;
	global.tail = p;
}

void global_rmvLocalEnv(char *name){
	// remove the first
	FuncEnv *func;
	func = global_getLocalEnv(name);

	LocalEnv *rmv;

	rmv = func->subEnv;
	func->subEnv = rmv->next;

	fclose(rmv->func);
	fclose(rmv->var);

	rmv->func = NULL;
	rmv->var = NULL;

	free(rmv);
}

void global_end(void){
	while(global.head != NULL){
		while(global.tail->subEnv != NULL)
			global_rmvLocalEnv(global.tail->name);

		global_rmvFuncEnv();
	}

	fclose(global.order);
	fclose(global.libVar);
	fclose(global.libFunc);
	fclose(global.var);
	fclose(global.func);
	fclose(global.constants);

	global.order = NULL;
	global.libVar = NULL;
	global.libFunc = NULL;
	global.var = NULL;
	global.func = NULL;
	global.constants = NULL;
	
	global.head = NULL;
	global.tail = NULL;
}

static FuncEnv* global_getLocalEnv(char *name){
	if(global.head == global.tail)
		return global.head;

	FuncEnv *p;

	for(p = global.head; strcmp(p->name, name) != 0 && p != NULL; p = p->next);

	return p;
}

static FILE* global_getBuf(short bufId, char *name){
	// global
	switch(bufId){
		case ENV_LIB_VAR:     return global.libVar; break;
		case ENV_LIB_FUNC:    return global.libFunc; break;
		case ENV_GLOBAL_VAR:  return global.var; break;
		case ENV_GLOBAL_FUNC: return global.func; break;
		case ENV_CONSTANTS:   return global.constants; break;
	}

	// local
	FuncEnv *func;
	func = global_getLocalEnv(name);

	switch(bufId){
		case ENV_LOCAL_VAR:  return func->subEnv->var; break;
		case ENV_LOCAL_FUNC: return func->subEnv->func; break;
	}
}
