#ifndef lim_flags
#define lim_flags

// 7 + 2 + 1
#define BIG_FLAG 10

#define F_VERSION "-v","--version"
#define F_MANUAL  "-m","--manual"
#define F_HELP    "-h","--help"
#define F_VERBOSE "-V","--verbose"
#define F_NAME    "-n","--name"
#define F_REPLACE "-r","--replace"

void welcome(void); // called without arguments
void version(void);
void manual(void);
void help(char *flag);
void verbose(void);

#endif
