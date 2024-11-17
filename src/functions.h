#ifndef LIM_FUNCTIONS
#define LIM_FUNCTIONS

#include <stdio.h>
#include <stdbool.h>
#include "heads.h"

// MAIN
static void cleanup(void);

// TOOLS
bool tools_strcmp2(char *str, char *v0, char *v1);
bool tools_strcmp3(char *str0, char *str1);
FILE* tools_copyFile(FILE *org, char *dstName);
char* tools_rmvParen(char *word);
unsigned short tools_strlen2(char *word);
void tools_fcat(FILE *src, FILE *dest);
void tools_initDinStr(char **buf);
void tools_addDinStr(char *buf, char c);
void tools_endDinStr(char **buf, bool restart);
long tools_filelen(FILE *file);

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
void global_order(short code);
void global_getOrder(short *code);
void global_print(char *word, char *name, short bufId);
void global_rmvEnv(void);
GlobalEnv* global_get(void);
void global_end(void);
static FILE* global_getBuf(short bufId, char *name);

void refe_init(void);
void refe_add(char *table, char *func);
void refe_treeToQueue(void);
RefeQueue* refe_getAndRmvQueueItem(void);
void refe_endTree(void);
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

void scope_init(void);
void scope_add(char *word, short bufId);
FILE* scope_get(short bufId);
void scope_rmvLastComma(short bufId);
void scope_end(void);

void nick_init(void);
static void nick_upChar(long id);
static void nick_upAll(long last);
void nick_up(void);
char *nick_get(void);
void nick_end(void);

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
void cp_3_buildingGlobalScope(void);
void cp_x_mergingContentAndPackingLibrary(void);

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

// HEADER
char* head_init(void);
bool head_printTop(FILE *dest);
bool head_printScope(FILE *dest);
bool head_checkFuncList(char *word);
void head_initWord(void);
void head_addWord(char c);
char *head_getWord(void);
void head_endWord(bool restart);
FILE* head_getList(void);
void head_end(void);
static bool head_getFromOrigin(FILE *src, FILE *dest, long *ltell, long *ctell);

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

void info_welcome(void);
void info_version(void);
void info_helpList(void);
void info_help(char *flag);
void info_verbose(short mode, ...);
void info_license(void);
void info_rules(void);

#endif
