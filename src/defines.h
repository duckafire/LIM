#ifndef LIM_DEFINES
#define LIM_DEFINES

#include <math.h>

// TOOLS
#define ARRAY_LEN(a) (sizeof(a) / sizeof(a[0]))

// BUFFERS
#define REFE_TOTAL_BUF 10
#define FROMSRC_TOTAL_BUF 9
#define FUNC_ENV_TOTAL_BUF 4
#define FROMSRC_ORDER 0
#define REFE_FUNC 0
#define SCOPE_TOTAL_BUF 2
#define REFE3_TOQUEUE(k) refe_buildQueue(refe_tree[i]->k, ((i == 0) ? NULL : lim.lua_tabs[i - 1]))
#define pairs_init

#define SCOPE_BASE_REPLACE_LAST_COMMA(c)        \
	fseek(scope_get(SCOPE_BASE), -1, SEEK_CUR); \
	fputc(c, scope_get(SCOPE_BASE))

#define SCOPE_MERGE_BASE_WITH_BUF                             \
	SCOPE_BASE_REPLACE_LAST_COMMA('=');                       \
	t_fcat(scope_get(SCOPE_BASE), scope_get(SCOPE_FUNC_BUF)); \
	SCOPE_BASE_REPLACE_LAST_COMMA(' ')

#define PAIRS_UPDATE_ORDER(src)          \
	Queue *newPairs = NULL;              \
	pairs_newOrderQueue(src, &newPairs); \
	src = newPairs

#define IS_VALID_SCOPE(file) (ftell(file) > 7)

enum{
	SCOPE_FUNC_BUF, // original identifier of functions
	SCOPE_BASE,     // nicknames
};

enum{
	NICK_TO_LUA_HEAD_FUNC,// none is added
	NICK_TO_GLOBAL_IDENT, // add 'G': Ga, Gb, ...
	NICK_TO_LOCAL_IDENT,  // add 'L': La, Lb, ...
	NICK_TO_PARAMETER,    // none is added
	NICK_TO_ALIGN_PARAM,  // add 'P': Pa, Pb, ...
};

// CHECK-FLAGS
// the bigger size between all flags
#define LARGEST_FLAG 12
// flags that print text on screen
#define INFO_FLAGS 2

// COMPACTION PROCESS
#define SEPARATOR(f) fputc('\n',f)
#define REVERTE_FORDER           \
	temp = cur->bufs[0];         \
	cur->bufs[0] = cur->bufs[1]; \
	cur->bufs[1] = temp;
#define SPACE_BETWEEN         \
	if(spaceBetween){         \
		fputc(' ', finalCtt); \
		spaceBetween = false; \
	}

// CONTENT-TREATMENT

// it don't work correct if `n < 2`
#define INT_LEN(n) ((n<2)?1:((int)((ceil(log10(n)))*sizeof(char))))
#define IS_FUNC_TYPE(t) (     \
	t == TYPE_LIB_FUNC     || \
	t == TYPE_GLOBAL_FUNC  || \
	t == TYPE_LOCAL_FUNC_0 || \
	t == TYPE_ANONYMOUS       \
)
#define IS_BUF_TYPE_FALSE_CONST(b) ( \
	b == TYPE_ANONYMOUS   || \
	b == TYPE_PARAM_CONST || \
	b == TYPE_PARAM_END      \
)
#define ADJ_LOCAL_TYPE(t) (t - TYPE_LOCAL_FUNC_0)
#define IS_LIB_OR_GLOBAL_TYPE(t) ( \
	t != TYPE_LOCAL_FUNC_0  && \
	t != TYPE_LOCAL_VAR_1   && \
	t != TYPE_LOCAL_PSELF_2 && \
	t != TYPE_LOCAL_PALIG_3    \
)

#define IS_FROM_SRCFILE(c) (c != TYPE_FROM_LUA && c != TYPE_FROM_HEAD)

enum{
	PREFIX_NONE = -1,
	PREFIX_G,          // _G
	PREFIX_FUNCTION,   // function
	PREFIX_LOCAL,      // local
	PREFIX_LOCAL_FUNC, // local function
	PREFIX_LUA_TABLE,  // table, math, ...
	PREFIX_PARAMETER,  // [`function` | func. ident.] <(>
	PREFIX_COMMA,      // <local> <ident0> <,> [ident1 <,> [identn <,>]] [=] ...
	PREFIX_POS_COMMA,  // [ident1, [iden2, [identn]]]
};

enum{
	// GlobalEnv
	TYPE_NONE = -1,   // this is not a array slot
	// ORDER (= 0) is not used in "type system"
	TYPE_LIB_FUNC = 1,// function <name>
	TYPE_LIB_VAR,     // _G.<name>
	TYPE_GLOBAL_FUNC, // local function (root env)
	TYPE_GLOBAL_VAR,  // loca <name> (~)
	TYPE_USE_OR_CALL, // func() || var + var1
	TYPE_CONSTANT,    // repeat, table, and
	TYPE_FROM_LUA,    // math, pairs, .random, ...
	TYPE_FROM_HEAD,   // function from "List Partition", from "header.lim"
	// both are converted to TYPE_CONSTANT
	TYPE_ANONYMOUS,   // function() end (it will be converted to TYPE_CONSTANT)
	TYPE_PARAM_CONST, // ',' or ')'
	TYPE_PARAM_END,   // ')'
	// FuncEnv; the number in last
	// specify the index of `bufs`
	TYPE_LOCAL_FUNC_0,  // local function (func env)
	TYPE_LOCAL_VAR_1,   // local <name> (~)
	TYPE_LOCAL_PSELF_2, // itself parameteres:     [(] [par0] [,] [parn] <)>
	TYPE_LOCAL_PALIG_3, // from aligned functions: ~
};

// HEADER
#define HEADER_BLOCK_NH  "> Blocked by flag: (-nh) --no-header"
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

// STAGE-PRODUCT
#define SP_BASE(id)            \
	if(flags.untilStage != id) \
		return false;          \
	info_verbose(VM_BREAK_STAGE, id)

#define NONE_BUFFER_TO_FREE if(flags.verbose) puts("\n")

#define EXP_BUF toexport=tmpfile()

#define EXP_NOW \
	t_copyAndExportFile(toexport); \
	fclose(toexport)

#define TOEXP_PRINT(str, ...) fprintf(toexport, str, ##__VA_ARGS__)
#define TOEXP_FCAT(file) t_fcat(toexport, file)

// PRINT-TEXT/ERRORS (Error MeSsaGe)

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
	VM_STAGE,
	VM_BREAK_STAGE,
	VM_BREAK_FREE,
	VM_PROCESS,
	VM_FREE,
	VM_BUFFER_INIT,
	VM_BUFFER_END,
	VM_HEADER,
};

#endif
