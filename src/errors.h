#ifndef lim_errors
#define lim_errors

// error code
enum {
	REPEAT_FLAG = 1,
	UNEXPECTED_FLAG,
	ARG_EXPECTED,
	INVALID_FLAG,
	FILES_NAMES_OVERFLOW
};

void repeatFlag(short pos, char *arg);
void unexpectedFlag(short pos, char *arg);
void argExpected(char *_r, char *requester);
void invalidFlag(char *arg);
void filesNamesOverflow(void);

#endif
