#ifndef lim_errors
#define lim_errors

// error code
enum {
	E_REPEAT_FLAG = 1,
	E_UNEXPECTED_FLAG,
	E_ARG_EXPECTED,
	E_INVALID_FLAG,
	E_FILES_NAMES_OVERFLOW
};

void repeatFlag(short pos, char *arg);
void unexpectedFlag(short pos, char *arg);
void argExpected(char *_r, char *requester);
void invalidFlag(char *arg);
void filesNamesOverflow(void);

#endif
