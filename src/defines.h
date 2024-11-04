#ifndef LIM_DEFINES
#define LIM_DEFINES

#include <math.h>

// TOOLS
#define ARRAY_LEN(a) (sizeof(a) / sizeof(a[0]))

// BUFFERS
enum{
	ENV_CONSTANT,
	ENV_LIB_VAR,
	ENV_LIB_FUNC,
	ENV_GLOBAL_VAR,
	ENV_GLOBAL_FUNC,
	ENV_LOCAL_FUNC,
	ENV_LOCAL_VAR,
};

// CHECK-FLAGS
#define LARGEST_FLAG 12
#define INFO_FLAGS 5

// COMPACTION PROCESS
// it don't work correct if `n` is equal zero or one
#define INT_LEN(n) ((int)((ceil(log10(n)))*sizeof(char)))

enum{
	PREFIX_NONE,
	PREFIX_ANONYMOUS,
	PREFIX_GLOBAL,
	PREFIX_LOCAL,
	PREFIX_LIB_FUNC,
	PREFIX_LIB_VAR,
	PREFIX_GLOBAL_FUNC,
	PREFIX_GLOBAL_VAR,
	PREFIX_LOCAL_FUNC,
	PREFIX_LOCAL_VAR,
};

// CONTENT-TREATMENT
#define FGETC (c = fgetc(gf_origin))

enum{
	TYPE_CONSTANT,
	TYPE_LIB_FUNC,
	TYPE_LIB_VAR,
	TYPE_GLOBAL_FUNC,
	TYPE_GLOBAL_VAR,
	TYPE_LOCAL_FUNC,
	TYPE_LOCAL_VAR,
};

// PRINT-TEXT/ERRORS
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
