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

	// a chain with the functions environments
	// and them sub-environments
	struct FuncEnv *head, *tail;
}GlobalEnv;

#endif
