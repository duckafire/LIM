#ifndef LIM_DEFINES
#define LIM_DEFINES

#include <math.h>

// TOOLS
#define ARRAY_LEN(a) (sizeof(a) / sizeof(a[0]))

// BUFFERS

// CHECK-FLAGS
// the bigger size between all flags
#define LARGEST_FLAG 12
// flags that print text on screen
#define INFO_FLAGS 5

// COMPACTION PROCESS
// it don't work correct if `n < 2`
#define INT_LEN(n) ((n<2)?1:((int)((ceil(log10(n)))*sizeof(char))))

// used set a "spectation" after
// read a specify word
enum{
	PREFIX_NONE = -1, 
	// for checkPrefixNow
	PREFIX_ANONYMOUS,      // functions() end
	PREFIX_GLOBAL_FUNC,    // function (in root enviroment) 
	PREFIX_GLOBAL_VAR,     // variable/table (~)
	PREFIX_LOCAL_FUNC,     // function (inside function)
	PREFIX_LOCAL_VAR,      // variable/table (~)
	// for checkPrefixNextCycle
	PREFIX_GLOBAL,   // <local> <function <name> | <variable> [=] [value]>
	PREFIX_LOCAL,    // ~
	PREFIX_LIB_FUNC, // function declared without `local`
	PREFIX_LIB_VAR,  // tables and variables prefixed by `_G.`
};

// identify content from lua
enum{ // B = Block
	LUA_NONE_KW = -1, // it is not from lua
	LUA_NOB,          // it is a lua keyword
	LUAB_CLOSE,       // it is "end" (block)
	LUAB_OPEN,        // it can open a block
	LUA_FUNC,         // ipairs, tostring, ...
};

// CONTENT-TREATMENT
#define FGETC (c=fgetc(gf_origin))

// used for to get a specify buffer for
// store a word
enum{
	TYPE_NONE = -1,
	TYPE_CONSTANT,
	TYPE_FROM_LUA, // tables and functions
	TYPE_LIB_FUNC,
	TYPE_LIB_VAR,
	TYPE_GLOBAL_FUNC,
	TYPE_GLOBAL_VAR,
	TYPE_LOCAL_FUNC,
	TYPE_LOCAL_VAR,
	TYPE_USE_OR_CALL,
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
