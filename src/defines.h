#ifndef LIM_DEFINES
#define LIM_DEFINES

// MAIN
#define HEADS \
	#include "defines.h" \
	#include "variables.h" \
	#include "functions.h"

#define HEADS_BELOW \
	#include "../defines.h" \
	#include "../variables.h" \
	#include "../functions.h"

// TOOLS
#define LIM_VERSION "v1.0.0-alpha"
#define ARRAY_LEN(a) (sizeof(a) / sizeof(a[0]))

// BUFFERS
#define UNINT unsigned int

// CHECK-FLAGS
#define LARGEST_FLAG 12
#define INFO_FLAGS 5

// INPUT
#define IS_NUM(a) (a >= '0' && a <= '9')
#define IS_BIN(a) (a <= ' ' || a == 127)
#define IS_CHR(a) ( \
	(a >= 'a' && a <= 'z') || \
	(a >= 'A' && a <= 'Z') || \
	a == '_' \
)

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
