#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "heads.h"

static GlobalEnv fromsrc;
static BinaryNode *refe_tree[REFE_TOTAL_BUF];
static Queue *refe_queue;
static FILE *scope[2];
static Queue *pairs[2];
static FuncEnv *local;

// this is not a buffer, but there is
// not other for to put this...
static char nickFirst, nickLast;
static char *nick = NULL, *nickPrefixed = NULL;
static const char nickPrefixOptions[5] = {'\0', 'G', 'L', '\0', 'P'};



////////// GLOBAL ENVORINMENTS //////////

void fromsrc_init(void){
	for(short i = 0; i < FROMSRC_TOTAL_BUF; i++)
		fromsrc.bufs[i] = tmpfile();
	
	fromsrc.head = NULL;
	fromsrc.tail = NULL;
}

void fromsrc_newEnv(char *name){
	// insert in chain end
	FuncEnv *new;
	new = malloc(sizeof(FuncEnv));
	new->name = malloc(sizeof(name));
	new->next = NULL;

	for(short i = 0; i < FUNC_ENV_TOTAL_BUF; i++)
		new->bufs[i] = tmpfile();

	// the '(' is included
	strcpy(new->name, name);

	if(fromsrc.head == NULL){
		fromsrc.head = new;
		fromsrc.tail = new;
		return;
	}

	fromsrc.tail->next = new;
	fromsrc.tail = new;
}

void fromsrc_order(short code){
	fwrite(&code, sizeof(short), 1, fromsrc.bufs[FROMSRC_ORDER]);
}

bool fromsrc_getOrder(short *code){
	return (fread(code, sizeof(short), 1, fromsrc.bufs[FROMSRC_ORDER]) > 0);
}

void fromsrc_write(char *word, char *name, short bufId){
	// `name == NULL` for write in a global buffer
	fprintf( fromsrc_getBuf(bufId, name), "%s\n", word);
}

void fromsrc_rmvEnv(void){
	// remove the last
	if(fromsrc.head == fromsrc.tail){
		free(fromsrc.head);
		fromsrc.head = NULL;
		fromsrc.tail = NULL;
		return;
	}

	// get element from chain
	FuncEnv *p;
	for(p = fromsrc.head; p->next->next != NULL; p = p->next);

	fclose(p->next->bufs[0]);
	fclose(p->next->bufs[1]);
	free(p->next);

	p->next = NULL;
	fromsrc.tail = p;
}

GlobalEnv* fromsrc_get(void){
	return &fromsrc;
}

FILE* fromsrc_getBuf(short bufId, char *name){
	// global
	if(IS_BUF_TYPE_FALSE_CONST(bufId))
		bufId = TYPE_CONSTANT;
	
	if(IS_LIB_OR_GLOBAL_TYPE(bufId))
		return fromsrc.bufs[bufId];
	
	// local
	FuncEnv *f;

	if(fromsrc.head == fromsrc.tail)
		f = fromsrc.head;
	else
		for(f = fromsrc.head; f != NULL && strcmp(f->name, name) != 0; f = f->next);

	return f->bufs[ ADJ_LOCAL_TYPE(bufId) ];
}

void fromsrc_fseekSetAll(void){
	for(short i = 0; i < FROMSRC_TOTAL_BUF; i++)
		fseek(fromsrc.bufs[i], 0, SEEK_SET);

	fromsrc_fseekSetAllLocal(fromsrc.head);
}

void fromsrc_end(void){
	while(fromsrc.head != NULL)
		fromsrc_rmvEnv();
	
	fromsrc.head = NULL;
	fromsrc.tail = NULL;

	for(short i = 0; i < FROMSRC_TOTAL_BUF; i++){
		fclose(fromsrc.bufs[i]);
		fromsrc.bufs[i] = NULL;
	}
}

static void fromsrc_fseekSetAllLocal(FuncEnv *local){
	if(local == NULL)
		return;

	fromsrc_fseekSetAllLocal(local->next);

	fseek(local->bufs[0], 0, SEEK_SET);
	fseek(local->bufs[1], 0, SEEK_SET);
}




////////// REFE //////////

void refe_init(void){
	for(short i = 0; i < REFE_TOTAL_BUF; i++)
		refe_tree[i] = mm_treeInit('m');
}

void refe_add(char *table, char *func){
	if(table == NULL)
		mm_treeNewNode(refe_tree[REFE_FUNC], func[0], NULL, func, true);
	else if(func == NULL)
		mm_treeNewNode(refe_tree[REFE_FUNC], table[0], table, NULL, true);
	else
		mm_treeNewNode(refe_getBuf(table[0]), func[1], NULL, func, true);
}

static BinaryNode* refe_getBuf(char firstChar){
	switch(firstChar){
		case 'c': return refe_tree[1]; // courotine
		case 'd': return refe_tree[2]; // debug
		case 'i': return refe_tree[3]; // io
		case 'm': return refe_tree[4]; // math
		case 'o': return refe_tree[5]; // os
		case 'p': return refe_tree[6]; // package
		case 's': return refe_tree[7]; // string
		case 't': return refe_tree[8]; // table
		case 'u': return refe_tree[9]; // utf8
		default: refe_tree[0]; // func
	}
}

void refe_initQueueAndEndTree(void){
	for(short i = 0; i < REFE_TOTAL_BUF; i++){
		REFE3_TOQUEUE(left);
		REFE3_TOQUEUE(right);
		REFE3_TOQUEUE(next);
	}

	for(short i = 0; i < REFE_TOTAL_BUF; i++){
		mm_treeEnd(&(refe_tree[i]));
		refe_tree[i] = NULL;
	}
}

static void refe_buildQueue(BinaryNode *root, char *origin){
	if(root == NULL)
		return;

	refe_buildQueue(root->left,  origin);
	refe_buildQueue(root->right, origin);
	refe_buildQueue(root->next,  origin);

	if(root->content[1] == NULL) // only "table"
		mm_queueInsertItem(&refe_queue, root->quantity, root->content[0], NULL, true);
	else
		mm_queueInsertItem(&refe_queue, root->quantity, origin, root->content[1], true);
}

Queue* refe_getAndRmvQueueItem(void){
	// the value returned need to be freed
	Queue *toRemove;

	toRemove = refe_queue;

	if(refe_queue != NULL)
		refe_queue = refe_queue->next;

	return toRemove;
}



////////// SCOPE //////////

void scope_init(void){
	for(short i = 0; i < SCOPE_TOTAL_BUF; i++)
		scope[i] = tmpfile();

	fputs("local ", scope[SCOPE_BASE]);
}

void scope_add(char *word, short bufId){
	fprintf(scope[bufId], "%s,", word);
}

FILE *scope_get(short bufId){
	return scope[bufId];
}

void scope_end(void){
	for(short i = 0; i < SCOPE_TOTAL_BUF; i++){
		fclose(scope[i]);
		scope[i] = NULL;
	}
}



////////// NICK //////////

void nick_init(bool toFuncs){
	nick = malloc(sizeof(char) * 2);

	if(toFuncs){
		nickFirst = 'A';
		nickLast  = 'Z';
	}else{
		nickFirst = 'a';
		nickLast  = 'z';
	}

	nick[0] = nickFirst;
	nick[1] = '\0';
}

void nick_up(void){
	nick_upAll( strlen(nick) - 1 );
}

static void nick_upAll(long last){
	if(last > -1){
		if(nick[last] == nickLast){
			nick[last] = nickFirst;
			nick_upAll(last - 1);
			return;
		}

		nick[last]++;
		return;
	}

	char *buf;
	buf = nick;

	last = strlen(nick) + 1;
	nick = malloc(last);
	
	nick[0] = nickFirst;
	strcat(nick, buf);
	free(buf);
}

char *nick_get(short mode){
	free(nickPrefixed);
	nickPrefixed = malloc(strlen(nick) + sizeof(char) * 2);

	nickPrefixed[0] = nickPrefixOptions[ mode ];
	nickPrefixed[1] = '\0';

	strcat(nickPrefixed, nick);

	return nickPrefixed;
}

void nick_end(void){
	free(nick);
	nick = NULL;

	free(nickPrefixed);
	nickPrefixed = NULL;
}



////////// PAIRS //////////

// this is only a convention
// #define pairs_init

bool pairs_add(bool fromSrcFile, unsigned short quantity, char *nick, char *ident){
	// 0 (A-Z): functions from Lua and from "header.lim"
	// 1 (a-z): variables, tables and functions declared with `local`, in root env.
	return mm_queueInsertItem(&pairs[ fromSrcFile ], quantity, nick, ident, false);
}

void pairs_updateQuantity(char *string){
	pairs_upItemQtt(pairs[1], string);
}

static void pairs_upItemQtt(Queue *item, char *string){
	if(item == NULL)
		return;

	if(t_strcmp3(item->content[1], string)){
		(item->quantity)++;
		return;
	}

	pairs_upItemQtt(item->next, string);
}

void pairs_updateOrder(void){
	PAIRS_UPDATE_ORDER(pairs[1]);
}

static void pairs_newOrderQueue(Queue *src, Queue **dest){
	if(src == NULL)
		return;

	pairs_newOrderQueue(src->next, dest);

	mm_queueInsertItem(dest, src->quantity, src->content[0], src->content[1], true);
	mm_treeFreeNodeAndQueueItem(NULL, src);
}

Queue* pairs_get(bool fromSrcFile){
	return pairs[ fromSrcFile ];
}

void pairs_end(void){
	pairs_endQueue(pairs[0]);
	pairs_endQueue(pairs[1]);
}

static void pairs_endQueue(Queue *item){
	if(item == NULL)
		return;

	pairs_endQueue(item->next);

	for(short i = 0; i < 2; i++)
		if(item->content[i] != NULL)
			free(item->content[i]);

	free(item);
}



////////// LOCAL //////////

// scope + pairs

void local_init(void){
	FuncEnv *p;

	local = fromsrc.head;

	for(p = local; p != NULL; p = p->next){
		p->scope = tmpfile();
		p->pairs = NULL; // it will be started in: local_pairsAdd

		fputs("local ", p->scope);
	}
}

void local_scopeAdd(FILE *cur, char *word){
	fprintf(cur, "%s,", word);
}

void local_scopeRmvLastComma(FuncEnv *cur){
	fseek(cur->scope, -1, SEEK_CUR);
	fputc(' ', cur->scope);
}

bool local_pairsAdd(Queue **pairs, char *nick, char *ident){
	return mm_queueInsertItem(pairs, 0, nick, ident, false);
}

void local_pairsUpdateQuantity(FuncEnv *cur, char *string){
	pairs_upItemQtt(local->pairs, string);
}

void local_pairsUpdateOrder(FuncEnv *cur){
	PAIRS_UPDATE_ORDER(local->pairs);
}

FuncEnv *local_get(void){
	return local;
}

void local_end(void){
	FuncEnv *p;

	for(p = local; p != NULL; p = p->next){
		fclose(p->scope);
		free(p->pairs);
	}
}
