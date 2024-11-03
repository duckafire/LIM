#ifndef LIM_STRUCT
#define LIM_STRUCT

#include <stdio.h>

struct LocalEnv {
	FILE *func;
	FILE *var; // and tables

	// a chain with local environments
	struct LocalEnv *next;
};

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

	// content that not will be compacted;
	// they are: numbers, booleans, `nil`,
	// strings, metamethods, key tables,
	// special characteres and key words
	FILE *constants;

	struct LocalEnv *head; // for remove all
	struct LocalEnv *tail; // for access the last
}GlobalEnv;

#endif
