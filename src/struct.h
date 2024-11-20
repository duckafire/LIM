#ifndef LIM_STRUCT
#define LIM_STRUCT

#include <stdio.h>
#include <stdbool.h>

// `extern` structure in file end

// flags used like arguments to Lim
typedef struct FlagsVar{
	bool verbose;  // false
	bool replace;  // false
	bool headfile; // true
	short untilStage; // 0 (all)
}FlagsVar;

// other arguments used to Lim
typedef struct LimVal{
	char *sourceFileName;  // "mandatory"
	char *destineFileName; // `sourceFileName`.lim
	FILE *sourceFile; // "mandatory"
	// `destineFile` is a local pointer (STAGE 5)
	char lua_tabs[9][10]; // used in "c..treatment" and "buffers"
}LimVal;

// always that a new function is found on
// root environment, a new element to
// "file buffer system" (with this structure)
// and added to "global buffer chain"
typedef struct FuncEnv{
	char *name;
	FILE *bufs[2];

	// next function environment
	struct FuncEnv *next;
}FuncEnv;

// for a single element; to separate and to store
// file content, based in it "type"
typedef struct{
	// 0 - order - original order of elements in script
	// 1 - libFunc - functions that will be added to library table: `function <name>()`
	// 2 - libVar - variables and tables that will be added to library table `_G. <name>`
	// 3 - globFunc - "global" functions: `local function <name>()`
	// 4 - globVar - "global" variables and tables `local <name>`
	// 5 - userOrCall - use of variables, or tables, and functions call
	// 6 - constants - content that not will be compacted; they are: numbers, booleans, `nil`, strings, metamethods, key tables, special characteres and key words
	// 7 - luaFunc - they will be used for create a "reference scope"
	// 8 - headFunc - list of function getted from "header.lim", in "List Patition"
	FILE *bufs[9];

	// a chain with the functions environments
	// and them sub-environments
	struct FuncEnv *head, *tail;
}GlobalEnv;

// this binary tree use characters like
// index; items with the same index are
// stored in the "same" node, in a queue
typedef struct BinaryNode{
	char id;
	char *content[2]; // 0: table/nickname; 1: function; ~
	unsigned short quantity; // used only by "refe"
	struct BinaryNode *left, *right, *next;
}BinaryNode;

typedef struct Queue{
	char *content[2];
	unsigned short quantity;
	struct Queue *next;
}Queue;

extern FlagsVar flags;
extern LimVal lim;

#endif
