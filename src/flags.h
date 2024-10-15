#ifndef lim_flags
#define lim_flags

// 7 + 2 + 1
#define LARGEST_FLAG 12

#define F_VERSION "-v","--version"
#define F_H_LIST  "-hl","--help-list"
#define F_HELP    "-h","--help"
#define F_VERBOSE "-V","--verbose"
#define F_NAME    "-n","--name"
#define F_REPLACE "-r","--replace"
#define F_LICENSE "-l","--license"

void welcome(void); // called without arguments
void version(void);
void helpList(void);
void help(char *flag);
void verbose(void);
void license(void);

#endif
