#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "heads.h"

static FILE *collect = NULL;
static char *ident = NULL; // identifier
static GlobalEnv global;
static RefeTree refe;
static LibScope scope;
static char *nick = NULL; // buffer be like

void buffers_atexit(void){
	if(collect != NULL)
		collect_end();

	if(ident != NULL)
		ident_end(0);

	if(global.head != NULL)
		global_end();

	if(refe.func != NULL)
		refe_endTree();

	if(refe.queue != NULL){
		RefeQueue *i;
		while((i = refe_getAndRmvQueueItem()) != NULL);
	}

	if(nick != NULL)
		nick_end();
}




////////// COLLECT //////////

void collect_init(void){
	collect = tmpfile();
}

void collect_add(char c){
	fputc(c, collect);
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
	fputc(code + '0', global.order);
	fputc('\n', global.order);
}

void global_print(char *word, char *name, short bufId){
	// `name == NULL` for print
	// in a global buffer
	FILE *buf;
	unsigned int i = 0;
	char *c = NULL;

	buf = global_getBuf(bufId, name);

	for(c = &word[i]; *c != '\0'; c = &word[++i])
		fputc(*c, buf);

	fputc('\n', buf);
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




////////// REFE //////////

void refe_init(void){
	refe.queue = NULL; // started in "refe_treeToQueue"
	NEW_REFE_ROOTS(refe.func);
	NEW_REFE_ROOTS(refe.courotine);
	NEW_REFE_ROOTS(refe.debug);
	NEW_REFE_ROOTS(refe.io);
	NEW_REFE_ROOTS(refe.math);
	NEW_REFE_ROOTS(refe.os);
	NEW_REFE_ROOTS(refe.package);
	NEW_REFE_ROOTS(refe.string);
	NEW_REFE_ROOTS(refe.table);
	NEW_REFE_ROOTS(refe.utf8);
}

void refe_add(char *table, char *func){
	char *content; // it will be free in `ends`
	content = malloc(strlen(func) + 1);
	strcpy(content, func);

	unsigned int last = strlen(content) - 1;
	if(content[last] == '(')
		content[last] = '\0';

	if(table == NULL){
		refe_newNode(refe.func, content[0], content);
		return;
	}

	refe_newNode(refe_getTableBuf(table), content[1], content);
}

static void refe_newNode(RefeNode *node, char id, char *content){
	if(id == node->id){
		if(tools_strcmp3(node->content, content))
			node->quantity++;
		else if(node->next == NULL)
			node->next = refe_createCell(content);
		else
			refe_newCell(node->next, content);

		return;
	}

	if(id < node->id){
		if(node->left == NULL){
			node->left = refe_createNode(id, content);
			return;
		}

		refe_newNode(node->left, id, content);
		return;
	}

	if(node->right == NULL){
		node->right = refe_createNode(id, content);
		return;
	}

	refe_newNode(node->right, id, content);
}

static void refe_newCell(RefeCell *cell, char *content){
	if(tools_strcmp3(cell->content, content)){
		cell->quantity++;
		return;
	}
	
	if(cell->next == NULL){
		cell = refe_createCell(content);
		return;
	}

	refe_newCell(cell->next, content);
}

static RefeNode* refe_createNode(char id, char *content){
	RefeNode *node;
	node = malloc(sizeof(RefeNode));
	node->id = id;
	node->next = NULL;
	node->left = NULL;
	node->right = NULL;
	node->quantity = 0;
	node->content = NULL;

	if(content != NULL){
		node->content = malloc(strlen(content) + 1);
		strcpy(node->content, content);
		node->quantity++;
	}

	return node;
}

static RefeCell* refe_createCell(char *content){
	RefeCell *cell;

	cell = malloc(sizeof(RefeCell));
	cell->quantity = 0;
	cell->content = content;
	cell->next = NULL;

	return cell;
}

static RefeNode* refe_getTableBuf(char *table){
	if(table == NULL) return refe.func;
	if(strcmp(table, "math")      == 0) return refe.math;
	if(strcmp(table, "string")    == 0) return refe.string;
	if(strcmp(table, "table")     == 0) return refe.table;
	if(strcmp(table, "debug")     == 0) return refe.debug;
	if(strcmp(table, "package")   == 0) return refe.package;
	if(strcmp(table, "utf8")      == 0) return refe.utf8;
	if(strcmp(table, "io")        == 0) return refe.io;
	if(strcmp(table, "os")        == 0) return refe.os;
	if(strcmp(table, "courotine") == 0) return refe.courotine;
}

void refe_treeToQueue(void){
	refe_subtreeToQueue(refe.func,      NULL);
	refe_subtreeToQueue(refe.courotine, "courotine");
	refe_subtreeToQueue(refe.debug,     "debug");
	refe_subtreeToQueue(refe.io,        "io");
	refe_subtreeToQueue(refe.math,      "math");
	refe_subtreeToQueue(refe.os,        "os");
	refe_subtreeToQueue(refe.package,   "package");
	refe_subtreeToQueue(refe.string,    "string");
	refe_subtreeToQueue(refe.table,     "table");
	refe_subtreeToQueue(refe.utf8,      "utf8");
}

RefeQueue* refe_getAndRmvQueueItem(void){
	// after the use, the pointer
	// returned need to be freed
	RefeQueue *itemRemoved;

	itemRemoved = refe.queue;

	if(refe.queue != NULL)
		refe.queue = refe.queue->next;

	return itemRemoved;
}

static void refe_subtreeToQueue(RefeNode *node, char *table){
	if(node == NULL)
		return;

	refe_subtreeToQueue(node->left, table);
	refe_subtreeToQueue(node->right, table);
	refe_subtreeQueueToMainQueue(node->next, table);
	

	if(node->content == NULL)
		return;

	char *content = NULL;

	content = malloc(strlen(node->content) + 1);
	strcpy(content, node->content);
	node->content = NULL;

	refe_createQueueItem(table, content, node->quantity);
}

static void refe_subtreeQueueToMainQueue(RefeCell *cell, char *table){
	if(cell == NULL)
		return;

	refe_subtreeQueueToMainQueue(cell->next, table);

	refe_createQueueItem(table, cell->content, cell->quantity);
}

static void refe_createQueueItem(char *origin, char *content, unsigned short quantity){
	RefeQueue *item;
	item = malloc(sizeof(RefeQueue));

	if(origin != NULL){
		item->origin = malloc(strlen(origin) + 1);
		strcpy(item->origin, origin);
	}else{
		item->origin = NULL;
	}

	item->content = content;
	item->quantity = quantity;

	if(refe.queue == NULL){
		refe.queue = item;
		return;
	}

	if(refe.queue->quantity < item->quantity){
		item->next = refe.queue;
		refe.queue = item;
		return;
	}

	refe_insertQueueItem(refe.queue, item);
}

static void refe_insertQueueItem(RefeQueue *cursor, RefeQueue *item){
	if(cursor->next == NULL){
		cursor->next = item;
		return;
	}

	if(cursor->next->quantity == item->quantity){
		unsigned int cursorLen = tools_strlen2(cursor->next->content) + tools_strlen2(item->origin);
		unsigned int itemLen = tools_strlen2(cursor->next->content) + tools_strlen2(item->origin);

		if(cursorLen < itemLen){
			cursor->next = item;
			return;
		}

		refe_insertQueueItem(cursor->next, item);
		return;
	}

	if(cursor->next->quantity < item->quantity){
		RefeQueue *buf;
		buf = cursor->next;

		cursor->next = item;
		item->next = buf;
		return;
	}

	refe_insertQueueItem(cursor->next, item);
}

void refe_endTree(void){
	refe_endNode(refe.func);
	refe_endNode(refe.math);
	refe_endNode(refe.string);
	refe_endNode(refe.table);
	refe_endNode(refe.debug);
	refe_endNode(refe.package);
	refe_endNode(refe.utf8);
	refe_endNode(refe.io);
	refe_endNode(refe.os);
	refe_endNode(refe.courotine);

	refe.func = NULL;
	refe.math = NULL;
	refe.string = NULL;
	refe.table = NULL;
	refe.debug = NULL;
	refe.package = NULL;
	refe.utf8 = NULL;
	refe.io = NULL;
	refe.os = NULL;
	refe.courotine = NULL;
}

static void refe_endNode(RefeNode *node){
	if(node == NULL)
		return;

	refe_endNode(node->left);
	refe_endNode(node->right);
	refe_endCell(node->next);

	if(node->content != NULL)
		free(node->content);
	free(node);
}

static void refe_endCell(RefeCell *cell){
	if(cell == NULL)
		return;

	refe_endCell(cell->next);

	if(cell->content != NULL)
		free(cell->content);
	free(cell);
}




////////// SCOPE //////////

void scope_init(void){
	scope.addr = tmpfile();
	scope.func = tmpfile();
	scope.var  = tmpfile();
}

void scope_add(char *word, short bufId){
	FILE *buf;
	buf = scope_get(bufId);

	unsigned short i = 0;
	char *c = NULL;
	for(c = &word[i]; *c != '\0'; c = &word[++i])
		fputc(*c, buf);

	fputc(',', buf);
}

FILE* scope_get(short bufId){
	switch(bufId){
		case SCOPE_ADDR: return scope.addr;
		case SCOPE_FUNC: return scope.func;
		case SCOPE_VAR:  return scope.var;
	}
}

void scope_rmvLastComma(short bufId){
	fseek(scope_get(bufId), -1, SEEK_CUR);
}

void scope_end(void){
	fclose(scope.addr);
	fclose(scope.func);
	fclose(scope.var);

	scope.addr = NULL;
	scope.func = NULL;
	scope.var  = NULL;
}




////////// NICK //////////

void nick_init(void){
	nick = malloc(sizeof(char) * 2);
	nick[0] = 'A';
	nick[1] = '\0';
}

static void nick_upChar(long id){
	if(nick[id] == '_'){
		nick[id] = 'M';
		return;
	}

	nick[id] += 1;

	if(nick[id] == 'L')
		nick[id] = '_';
}

static void nick_upAll(long last){
	if(last > -1){
		if(nick[last] == 'Z'){
			nick[last] = 'A';
			nick_upAll(last - 1);
			return;
		}

		nick_upChar(last);
		return;
	}

	char *buf;
	buf = nick;

	last = strlen(nick) + 1;
	nick = malloc(last);
	
	nick[0] = 'A';
	strcat(nick, buf);
	free(buf);
}

void nick_up(void){
	nick_upAll( strlen(nick) - 1 );
}

char *nick_get(void){
	return nick;
}

void nick_end(void){
	free(nick);
	nick = NULL;
}
