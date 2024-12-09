#ifndef LIM_FUNCTIONS
#define LIM_FUNCTIONS

#include <stdio.h>
#include <stdbool.h>
#include "heads.h"

// MAIN
static void cleanup(void);

// TOOLS
bool t_strcmp2(char *str, char *v0, char *v1);
bool t_strcmp3(char *str0, char *str1);
bool t_strcmp4(char *_str0, char *_str1);
FILE* t_copyFile(FILE *org, char *dstName);
char* t_rmvParen(char *word);
unsigned int t_strlen2(char *str);
void t_fcat(FILE *src, FILE *dest);
long t_filelen(FILE *file);
char* t_allocAndCopy(char *src);
void t_copyAndExportFile(FILE *src);
void t_getStringFromFile(FILE *src, char *c, char **string);
char* t_setAnonyFuncName(unsigned short *index);

// BUFFERS
void buffers_atexit(void);

void fromsrc_init(void);
void fromsrc_newEnv(char *name);
void fromsrc_order(short code);
bool fromsrc_getOrder(short *code);
void fromsrc_print(char *word, char *name, short bufId);
void fromsrc_rmvEnv(void);
GlobalEnv* fromsrc_get(void);
FILE* fromsrc_getBuf(short bufId, char *name);
void fromsrc_fseekSetAll(void);
void fromsrc_end(void);
static void fromsrc_fseekSetAllLocal(FuncEnv *local);

void refe_init(void);
void refe_add(char *table, char *_func);
void refe_treeToQueue(void);
Queue* refe_getAndRmvQueueItem(void);
void refe_endTree(void);
static BinaryNode* refe_getBuf(char firstChar);
static void refe_buildQueue(BinaryNode *root, char *origin);

void scope_init(void);
void scope_add(char *word, short bufId);
FILE* scope_get(short bufId);
void scope_rmvLastComma(short bufId);
void scope_end(void);

void scope_localAdd(char *name, char *word);
FuncEnv* scope_localGet(char *name);
void scope_localRmvLastComma(char *name);
static FuncEnv* scope_createLocal(char *name);
void scope_localEnd(void);
void scope_endItems(FuncEnv *item);

void nick_init(bool toFuncs);
static void nick_upChar(long id);
static void nick_upAll(long last);
void nick_up(void);
char *nick_get(void);
void nick_end(void);

void pairs_init(void);
void pairs_add(bool fromSrcFile, unsigned short quantity, char *nick, char *ident);
void pairs_updateQuantity(char *string);
void pairs_updateOrder(void);
char* pairs_get(bool fromSrcFile, char *string);
void pairs_printAll(FILE *dest, bool fromSrcFile);
void pairs_end(void);
static void pairs_upItemQtt(Queue *item, char *string);
static void pairs_newOrderQueue(Queue *src, Queue **dest);
static char* pairs_getNick(Queue *item, char *string);
static void pairs_printContent(Queue *item, FILE *dest);
static void pairs_endQueue(Queue *item);

// CHECK-FLAGS
void cf_setArgValues(int c, char *v[]);
void cf_single(void);
void cf_unexpected(void);
void cf_toCompaction(void);
void cf_invalid(void);
void cf_originName(void);
void cf_destineName_2(void);
static void cf_setDestineName(char *src, bool withPath);

// COMPACTION-PROCESS
void cp_0_checkAndOpenFiles(void);
bool cp_1_extractionFromOrigin(void);
bool cp_2_separateExtractedContent(void);
bool cp_3_buildingGlobalScope(void);
static void cp_3_buildingGlobalScope_functions(void);
static void cp_3_buildingGlobalScope_variablesAndTables(void);
bool cp_4_organizeAndCompact(void);
void cp_5_mergingContentAndPackingLibrary(void);

// CONTENT-TREATMENT
void ct_atexit(void);
bool ct_getIdentifier(char *c, bool isFirst);
char ct_clearSpaces(void);
void ct_specialCharTreatment(FILE *buf, char c);

short ct_readPrefix(char *word, short prefix, bool isRootEnv);
short ct_readCurWord(char *word);
short ct_setPrefix(char *word, short prefix, bool isRootEnv);
char* ct_checkAndCreateNewEnv(char *word, short typeCode, short *anonyId);
void ct_checkAndUpLayer(char *_word, unsigned short *code);
bool ct_checkLuaTabs(char *word);

static void clearComment(bool isLine);
static void saveString(FILE *buf, char signal);
static void saveBraces(FILE *buf);
static bool checkLuaKeywords(char *word, bool stage1);
static bool checkLuaFuncs(char *word);

// HEADER
char* header_init(void);
bool header_printBuf(FILE *dest, short buf);
bool header_checkFuncList(char *word);
FILE* header_getFuncList(void);
void header_end(void);
static bool header_getFromOrigin(FILE *src, short buf, long *ltell, long *ctell);

// MEMORY-MANIPULATION
void mm_stringInit(char **buf);
void mm_stringAdd(char **buf, char c);
void mm_stringEnd(char **buf, bool restart);

BinaryNode* mm_treeInit(char id);
void mm_treeNewNode(BinaryNode *root, char id, char *ctt0, char *ctt1, bool upQtt);
BinaryNode* mm_tree_GetContent(BinaryNode *root, char id);
void mm_treeFreeNodeAndQueueItem(BinaryNode *node, Queue *item);
void mm_treeEnd(BinaryNode **root);
static BinaryNode* mm_treeCreateNode(char id, char *ctt0, char *ctt1);
static void mm_treeInsertItem(BinaryNode  *node, BinaryNode *new, bool upQtt, bool toQueue);

void mm_queueInsertItem(Queue **head, unsigned short quantity, char *ctt0, char *ctt1);
static Queue* mm_queueNewItem(unsigned short quantity, char *ctt0, char *ctt1);
static void mm_queueContentsLength(unsigned int *v0, unsigned int *v1, Queue *i0, Queue *i1);
static void mm_queueInsertInBody(Queue *mom, Queue *son, Queue *new);

// PRINT-TEXT/ERRORS (ERror)
void er_repeatFlag(char *arg, short pos);
void er_unexpectedFlag(char *arg, short pos);
void er_argExpected(char *_r, char *requester);
void er_invalidFlag(char *arg, short pos);
void er_filesNamesOverflow(void);
void er_nameNotSpecified(void);
void er_nonExistentFile(char *name);
void er_fileAlreadyExistent(char *name);
void er_invalidSuffixToFlag(char *_r, char *requester, char *expected, char *gived);
void er_invalidArgToHelp(char *arg);

// PRINT-TEXT/INFORMATIONS
static void message(char n, ...);

void info_welcome(void);
void info_version(void);
void info_help(char *arg);
void info_verbose(short mode, ...);

// STAGE-RESULT
bool stageProduct_extractionFromOrigin(short id, FILE *extrCttBuf);
bool stageProduct_separateExtractedContent(short id);
bool stageProduct_buildingGlobalScope(short id);
bool stageProduct_organizeAndCompact(short id);

#endif
