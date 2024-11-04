#ifndef LIM_FUNCTIONS
#define LIM_FUNCTIONS

#include <stdio.h>
#include <stdbool.h>
#include "heads.h"

// MAIN
static void cleanup(void);

// TOOLS
bool strcmp2(char *str, char *v0, char *v1);
FILE* copyFile(FILE *org, char *dstName);

// BUFFERS
void buffers_atexit(void);

void collect_init(void);
void collect_add(char c);
FILE* collect_get(void);
void collect_end(void);

void ident_init(void);
void ident_add(char c);
char* ident_get(void);
void ident_end(short restart);

void global_init(void);
void global_newEnv(char *name);
void global_print(char *word, char *name, short bufId);
void global_rmvEnv(void);
void global_end(void);
static FuncEnv* global_getLocalEnv(char *name);
static FILE* global_getBuf(short bufId, char *name);

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
void cp_2_separateExtractedContent(void);
void cp_x_tempFinish(void);

// CONTENT-TREATMENT
bool getIdentifier(char *c, bool isFirst);
char clearSpaces(void);
void getSpecial(char c);
short contentType(char *word, short prefix);
short checkPrefixNow(char *word, short last);
short checkPrefixNextCycle(char *word, bool isRootEnv);

static void clearComment(bool isLine);
static void saveString(char signal);
static void saveBraces(void);
static bool saveDoubleSignal(char sig_0, char sig_1);
static short checkLuaKeywords(char *word);

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
