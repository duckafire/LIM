#ifndef lim_messages
#define lim_messages

#include <stdbool.h>

// ERRORS
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

void repeatFlag(char *arg, short pos);
void unexpectedFlag(char *arg, short pos);
void argExpected(char *_r, char *requester);
void invalidFlag(char *arg, short pos);
void filesNamesOverflow(void);
void nameNotSpecified(void);
void nonExistentFile(char *name);
void fileAlreadyExistent(char *name);

// FLAGS
#define LARGEST_FLAG 12

#define F_VERSION "-v","--version"
#define F_H_LIST  "-hl","--help-list"
#define F_HELP    "-h","--help"
#define F_VERBOSE "-V","--verbose"
#define F_NAME    "-n","--name"
#define F_REPLACE "-r","--replace"
#define F_LICENSE "-l","--license"

static void message(char n, ...);
static bool verboseMsg(short value, char *msg);

void welcome(void); // called without arguments
void version(void);
void helpList(void);
void help(char *flag);
void verbose(void);
void license(void);

#endif
