#ifndef LIM_STRUCT
#define LIM_STRUCT

#include <stdio.h>
#include <stdbool.h>
#include "defines.h"

// `extern` STRUCTURES IN FILE END

// flags used like arguments to Lim
typedef struct FlagsVar{
	bool verbose;     // false
	bool replace;     // false
	bool headfile;    // true
	short untilStage; // 0 (all)
}FlagsVar;

// other arguments used to Lim
typedef struct LimVal{
	char *sourceFileName;  // "mandatory"
	char *destineFileName; // `sourceFileName`.lim
	FILE *sourceFile;      // "mandatory"
	// `destineFile` is a local pointer (STAGE 6)
	char lua_tabs[9][10]; // used in "cont. treat." and "buffers"
}LimVal;

// to functions declared in "root env."
typedef struct FuncEnv{
	char *name;

	// 0: `local` variables and tables
	// 1: `~` functions
	// 2: itself parameters
	// 3: parameters from its functions
	FILE *bufs[FUNC_ENV_TOTAL_BUF];

	// next function environment
	struct FuncEnv *next;
}FuncEnv;

// buffers to content extracted from source file
typedef struct{
	// 0 - order      - order in that the content were divided in these buffers
	// 1 - libFunc    - declared no prefix `local`
	// 2 - libVar     - ALWAYS prefixed by `_G.`
	// 3 - globFunc   - declared with `local`, in root env.
	// 4 - globVar    - ~
	// 5 - userOrCall - use of variables, tables, and functions
	// 6 - constants  - numbers, booleans, `nil`, strings, metamethods, key tables, special characteres and key words
	// 7 - luaFunc    - default function from lua
	// 8 - headFunc   - user function, specified in "header.lim"
	FILE *bufs[FROMSRC_TOTAL_BUF];

	// queue
	struct FuncEnv *head, *tail;
}GlobalEnv;

typedef struct BinaryNode{
	char id;
	char *content[2];
	unsigned short quantity;
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
