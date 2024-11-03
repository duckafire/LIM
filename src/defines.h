#ifndef LIM_DEFINES
#define LIM_DEFINES

// TOOLS
#define ARRAY_LEN(a) (sizeof(a) / sizeof(a[0]))

// BUFFERS
#define ENV_LIB_VAR 0
#define ENV_LIB_FUNC 1
#define ENV_GLOBAL_VAR 2
#define ENV_GLOBAL_FUNC 3
#define ENV_LOCAL_FUNC 4
#define ENV_LOCAL_VAR 5
#define ENV_CONSTANTS 6

// CHECK-FLAGS
#define LARGEST_FLAG 12
#define INFO_FLAGS 5

// CONTENT-TREATMENT
#define FGETC (c = fgetc(gf_origin))

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
