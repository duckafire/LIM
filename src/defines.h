#ifndef LIM_DEFINES
#define LIM_DEFINES

#include <math.h>

// TOOLS
#define ARRAY_LEN(a) (sizeof(a) / sizeof(a[0]))

// BUFFERS
#define NEW_REFE_ROOTS(node) NEW_REFE_NODE(node, 'm', NULL)

#define NEW_REFE_NODE(node, chr, ctt)           \
	node = malloc(sizeof(RefeNode));            \
	node->id = chr;                             \
	node->next = NULL;                          \
	node->left = NULL;                          \
	node->right = NULL;                         \
	node->quantity = 0;                         \
	if(ctt != NULL){                            \
		node->content = malloc(strlen(ctt) + 1);\
		strcpy(node->content, ctt);             \
	}else                                       \
		node->content = NULL;

#define NEW_REFE_CELL(cell, ctt)    \
	cell = malloc(sizeof(RefeCell));\
	cell->quantity = 0;             \
	cell->content = content;        \
	cell->next = NULL;

// CHECK-FLAGS
// the bigger size between all flags
#define LARGEST_FLAG 12
// flags that print text on screen
#define INFO_FLAGS 5

// COMPACTION PROCESS

// CONTENT-TREATMENT
#define FGETC (c=fgetc(gf_origin))

// it don't work correct if `n < 2`
#define INT_LEN(n) ((n<2)?1:((int)((ceil(log10(n)))*sizeof(char))))

enum{
	PREFIX_NONE = -1,
	PREFIX_G,          // _G
	PREFIX_FUNCTION,   // function
	PREFIX_LOCAL,      // local
	PREFIX_LOCAL_FUNC, // local function
	PREFIX_LUA_TABLE   // table, math, ...
};

enum{
	TYPE_NONE = -1,
	TYPE_CONSTANT,    // repeat, table, and
	TYPE_ANONYMOUS,   // function() end
	TYPE_USE_OR_CALL, // func() || var + var1
	TYPE_FROM_LUA,    // math, pairs, .random, ...
	TYPE_LIB_FUNC,    // functio <name>
	TYPE_LIB_VAR,     // _G.<name>
	TYPE_GLOBAL_FUNC, // local function (root env)
	TYPE_GLOBAL_VAR,  // loca <name> (~)
	TYPE_LOCAL_FUNC,  // local function (func env)
	TYPE_LOCAL_VAR,   // local <name> (~)
};

// PRINT-TEXT/ERRORS (Error MeSsaGe)
#define E_MSG_FORMAT_0 "[LIM] %s: \"%s\" (#%d)\n\n"
#define E_MSG_FORMAT_1 "[LIM] %s: \"%s\"\n\n"
#define E_MSG_FORMAT_2 "[LIM] %s.\n\n"

#define E_MSG_1 "Invalid use of the flag (repetition)"
#define E_MSG_2 "Unexpected use of the flag"
#define E_MSG_3 "Argument expected after flag"
#define E_MSG_4 "Invalid flag"
#define E_MSG_5 "Many files were specified (max: 1)"
#define E_MSG_6 "File name (origin) was not specified"
#define E_MSG_7 "Non-existent file"
#define E_MSG_8 "File already existent (try: -r, --replace)"

// PRINT-TEXT/INFORMATIONS
#define F_VERSION "-v","--version"
#define F_H_LIST  "-hl","--help-list"
#define F_HELP    "-h","--help"
#define F_VERBOSE "-V","--verbose"
#define F_NAME    "-n","--name"
#define F_REPLACE "-r","--replace"
#define F_LICENSE "-l","--license"
#define F_RULES   "-R","--rules"

#endif
