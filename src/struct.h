#ifndef LIM_STRUCT
#define LIM_STRUCT

#include <stdio.h>

// always that a new function is finded on
// root environment, a new element to
// "file buffer system" (with this structure)
// and added to "global buffer chain"
typedef struct FuncEnv{
	char *name;

	FILE *func;
	FILE *var; // and tables

	// next function environment
	struct FuncEnv *next;
}FuncEnv;

// for a single element; to separate and to store
// file content, based in it "type"
typedef struct{
	// original order of elements
	// in script
	FILE *order;

	// variables and tables that will
	// be added to library table
	// `_G. <name>`
	FILE *libVar;

	// functions that will be added
	// to library table:
	// `function <name>()`
	FILE *libFunc;

	// "global" variables and tables
	// `local <name>`
	FILE *var;

	// "global" functions:
	// `local function <name>()`
	FILE *func;

	// use of variables, or tables, and
	// functions call
	FILE *useOrCall;

	// content that not will be compacted;
	// they are: numbers, booleans, `nil`,
	// strings, metamethods, key tables,
	// special characteres and key words
	FILE *constants;
	
	// they will be used for create a
	// "reference scope"
	FILE *luaFunc;

	// list of function getted from
	// "header.lim", in "List Patition"
	FILE *headFunc;

	// a chain with the functions environments
	// and them sub-environments
	struct FuncEnv *head, *tail;
}GlobalEnv;

// referece trees
typedef struct RefeCell{
	char *content;
	unsigned short quantity;
	struct RefeCell *next;
}RefeCell;

typedef struct RefeNode{
	char id;
	char *content;
	unsigned short quantity;
	struct RefeCell *next;
	struct RefeNode *left, *right;
}RefeNode;

typedef struct RefeQueue{
	char *origin; // math, string; NULL = none ("func")
	char *content;
	unsigned short quantity;
	struct RefeQueue *next;
}RefeQueue;

typedef struct RefeTree{
	RefeQueue *queue;

	RefeNode *func;

	RefeNode *math;
	RefeNode *string;
	RefeNode *table;
	RefeNode *debug;
	RefeNode *package;
	RefeNode *utf8;
	RefeNode *io;
	RefeNode *os;
	RefeNode *courotine;
}RefeTree;

typedef struct CompactPair{
	char id;
	char *nick;
	char *ident;
	struct CompactPair *left, *right, *next;
}CompactPair;

typedef struct{
	// value of function references (address):
	// A,B=math.random,tonumber
	FILE *addr;

	FILE *func;
	FILE *var;
}LibScope;

typedef struct{
	// content that will be placed in top
	// of the file, outside library
	// environment/pack, like credits
	FILE *top;

	// already content compacted, that will be
	// placed after library scopes
	FILE *scope;

	// list of function that will receive
	// a reference, like lua functions
	FILE *list;

	// store words that were getted from header
	// file, like buffer "ident", but it is
	// specify to header
	char *word;
}HeaderFile;

#endif
