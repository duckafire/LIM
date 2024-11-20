#ifndef LIM_DEFINES
#define LIM_DEFINES

#include <math.h>

// TOOLS
#define ARRAY_LEN(a) (sizeof(a) / sizeof(a[0]))

// BUFFERS
#define REFE_TOTAL_BUF 10
#define FROMSRC_TOTAL_BUF 9
#define FROMSRC_ORDER 0
#define REFE_FUNC 0
#define SCOPE_TOTAL_BUF 3
#define REFE3_TOQUEUE(k) refe_buildQueue(refe_tree[i]->k, ((i == 0) ? NULL : lim.lua_tabs[i - 1]))

enum{
	SCOPE_ADDR,
	SCOPE_FUNC,
	SCOPE_VAR,
};

enum{
	CTT_TABLE = 0,
	CTT_FUNC,
};

// CHECK-FLAGS
// the bigger size between all flags
#define LARGEST_FLAG 12
// flags that print text on screen
#define INFO_FLAGS 2

// COMPACTION PROCESS
#define TOKEN_TABLE_KEY '@'
#define TOKEN_FUNC_NAME '$'

// CONTENT-TREATMENT

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
	// GlobalEnv
	TYPE_NONE = -1,   // this is not a array slot
	// ORDER (= 0) is not used in "type system"
	TYPE_LIB_FUNC = 1,// functio <name>
	TYPE_LIB_VAR,     // _G.<name>
	TYPE_GLOBAL_FUNC, // local function (root env)
	TYPE_GLOBAL_VAR,  // loca <name> (~)
	TYPE_USE_OR_CALL, // func() || var + var1
	TYPE_CONSTANT,    // repeat, table, and
	TYPE_FROM_LUA,    // math, pairs, .random, ...
	TYPE_FROM_HEAD,   // function from "List Partition", from "header.lim"
	TYPE_ANONYMOUS,   // function() end (it will be converted to TYPE_CONSTANT)
	// FuncEnv; the number in last
	// specify the index of `bufs`
	TYPE_LOCAL_FUNC_0,  // local function (func env)
	TYPE_LOCAL_VAR_1,   // local <name> (~)
};

// HEADER
#define HEADER_BLOCK_NH  "> Blocked by flag: (-nh) --no-header"
#define HEADER_BLOCK_US  "> Blocked by flag: (-us) --until-stage"
#define HEADER_NOT_FOUND "> File not found."
#define HEADER_NONE_PART "> None partition was found."
#define HEADER_ONLY_TOP  "> Only the \"Top Partition\" was found."
#define HEADER_NO_LIST   "> \"List Partition\" not found."
#define HEADER_SUCCESS   "> Loaded sucessfully."

enum{
	HEADER_TOP,
	HEADER_SCOPE,
	HEADER_LIST,
};

// MEMORY-MANIPULATION

// PRINT-TEXT/ERRORS (Error MeSsaGe)
#define E_MSG_FORMAT_0 "<LIM> %s: \"%s\" (#%d)\n\n"
#define E_MSG_FORMAT_1 "<LIM> %s: \"%s\"\n\n"
#define E_MSG_FORMAT_2 "<LIM> %s.\n\n"
#define E_MSG_FORMAT_3 "<LIM> %s %s; expected %s, instead \"%s\"\n\n"

#define E_MSG_1  "Invalid use of the flag (repetition)"
#define E_MSG_2  "Unexpected use of the flag"
#define E_MSG_3  "Argument expected after flag"
#define E_MSG_4  "Invalid flag"
#define E_MSG_5  "Many files were specified (max: 1)"
#define E_MSG_6  "File name (origin) was not specified"
#define E_MSG_7  "Non-existent file"
#define E_MSG_8  "File already existent (try: -r, --replace)"
#define E_MSG_9  "Invalid suffix to flag:"
#define E_MSG_10 "Invalid argument to \"help\" flag"

// PRINT-TEXT/INFORMATIONS
#define F_VERSION "-v", "--version"
#define F_HELP    "-h", "--help"
#define F_VERBOSE "-V", "--verbose"
#define F_NAME    "-n", "--name"
#define F_REPLACE "-r", "--replace"
#define F_NO_HEAD "-nh","--no-header"
#define F_UNTIL_S "-us","--until-stage"

#define HELP_ARG_LIST "l", "list"
#define HELP_ARG_LICE "li","license"
#define HELP_ARG_RULE "r", "rules"
#define HELP_ARG_HEAD "h", "header"
#define HELP_ARG_SYNO "s", "synopsis"
#define HELP_ARG_LISY "ls", "list-syn"

enum{ // Verbose Mode
	VM_NORMAL,
	VM_TITLE,
	VM_BREAK,
	VM_START_PRO,
	VM_END_PRO,
	VM_START_BUF,
	VM_END_BUF,
};

#endif
