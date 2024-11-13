#ifndef LIM_FUNCTIONS
#define LIM_FUNCTIONS

#include <stdio.h>
#include <stdbool.h>
#include "heads.h"

// MAIN
static void cleanup(void);

// TOOLS
bool tools_strcmp2(char *str, char *v0, char *v1);
FILE* tools_copyFile(FILE *org, char *dstName);
char* tools_rmvParen(char *word);
unsigned short tools_strlen2(char *word);
char* tools_newStrNoParen(char *str);
bool tools_strcmpNoParen(char *_str0, char *_str1);

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
void global_order(short code, char *word);
void global_print(char *word, char *name, short bufId);
void global_rmvEnv(void);
GlobalEnv* global_get(void);
void global_end(void);
static FILE* global_getBuf(short bufId, char *name);

void refe_init(void);
void refe_add(char *table, char *func);
void refe_treeToQueue(void);
RefeQueue* refe_getQueue(void);
void refe_endTree(void);
void refe_endQueue(void);
static void refe_newNode(RefeNode *node, char id, char *content);
static void refe_newCell(RefeCell *cell, char *content);
static RefeNode* refe_createNode(char id, char *content);
static RefeCell* refe_createCell(char *content);
static RefeNode* refe_getTableBuf(char *table);
static void refe_subtreeToQueue(RefeNode *node, char *table);
static void refe_subtreeQueueToMainQueue(RefeCell *cell, char *table);
static void refe_createQueueItem(char *origin, char *content, unsigned short quantity);
static void refe_insertQueueItem(RefeQueue *cursor, RefeQueue *item);
static void refe_endNode(RefeNode *node);
static void refe_endCell(RefeCell *cell);
static void refe_freeQueueItem(RefeQueue *item);

// CHECK-FLAGS
void cf_setArgValues(int c, char *v[]);
void cf_single(void);
void cf_unexpected(void);
void cf_toCompaction(void);
void cf_invalid(void);
void cf_originName(void);
void cf_destineName_2(void);

// COMPACTION-PROCESS
void cp_0_checkAndOpenFiles(void);
void cp_1_extractionFromOrigin(void);
void cp_2_separateExtractedContent(void);
void cp_3_buildingGlobalScopes(void);
void cp_x_tempFinish(void);

// CONTENT-TREATMENT
void ct_atexit(void);
bool ct_getIdentifier(char *c, bool isFirst);
char ct_clearSpaces(void);
void ct_saveString(char signal);
void ct_getSpecial(char c);

short readPrefix(char *word, short prefix, bool isRootEnv);
short readCurWord(char *word);
short setPrefix(char *word, short prefix, bool isRootEnv);
char* checkAndCreateNewEnv(char *word, short typeCode);
void checkAndUpLayer(char *word, unsigned short *code);
bool checkLuaTabs(char *word);

static void clearComment(bool isLine);
static void saveBraces(void);
static bool saveDoubleSignal(char sig_0, char sig_1);
static bool checkLuaKeywords(char *word, bool stage1);
static bool checkLuaFuncs(char *word);

// PRINT-TEXT/ERRORS (ERror)
void er_repeatFlag(char *arg, short pos);
void er_unexpectedFlag(char *arg, short pos);
void er_argExpected(char *_r, char *requester);
void er_invalidFlag(char *arg, short pos);
void er_filesNamesOverflow(void);
void er_nameNotSpecified(void);
void er_nonExistentFile(char *name);
void er_fileAlreadyExistent(char *name);

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
