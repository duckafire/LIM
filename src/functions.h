#ifndef LIM_FUNCTIONS
#define LIM_FUNCTIONS

#include <stdio.h>
#include <stdbool.h>

// MAIN
static void cleanup(void);

// TOOLS
bool strcmp2(char *str, char *v0, char *v1);
FILE* copyFile(FILE *org, char *dstName);

// BUFFERS
void collect_init(void);
void collect_add(char c);
void collect_end(void);

// CHECK-FLAGS
void cf_setArgValues(int c, char *v[]);
void cf_single(void);
void cf_unexpected(void);
void cf_toCompaction(void);
void cf_invalid(void);
void cf_originName(void);
void cf_destineName_2(bool *dstUsingMalloc);

// COMPACTION-PROCESS
void cp_0_checkAndOpenFiles(void);
void cp_1_extractionFromOrigin(void);
void cp_x_tempFinish(void);

// INPUT
bool getIdentifier(char *c, bool isFirst);
char clearSpaces(void);
void getSpecial(char c);

static void clearComment(bool isLine);
static void saveString(char signal);
static void saveBraces(void);
static bool saveDoubleSignal(char sig_0, char sig_1);

// PRINT-TEXT/ERRORS
void repeatFlag(char *arg, short pos);
void unexpectedFlag(char *arg, short pos);
void argExpected(char *_r, char *requester);
void invalidFlag(char *arg, short pos);
void filesNamesOverflow(void);
void nameNotSpecified(void);
void nonExistentFile(char *name);
void fileAlreadyExistent(char *name);

// PRINT-TEXT/INFORMATIONS
static void message(char n, ...);
static bool verboseMsg(short value, char *msg);

void info_welcome(void);
void info_version(void);
void info_helpList(void);
void info_help(char *flag);
void info_verbose(void);
void info_license(void);
void info_rules(void);

#endif
