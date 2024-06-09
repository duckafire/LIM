#ifndef __STARTED_TIN
#define __STARTED_TIN

#define FLAGS_QTT 4
#define VERSION_CUR "0.1.4"
#define VERSION_LEN 5

#define ID0 "@0" // library function
#define ID1 "@1" // "local" or global (_G) variable (or table)
#define ID2 "@2" // metamethods
#define ID3 "@3" // reserved functions ("basic")
#define ID4 "@4" // reserved table, with its function (only if it is valid)
#define ID5 "@5" // strings
#define ID6 "@6" // internal functions
#define ID7 "@7" // all protected names called in code and all lua keywords


#include <stdio.h>


// main.c
void copyOrigin(void);
void cleanupMain(void);

static void messages(int flag);


// arg.c
int getFlags(int argc, char *argv[]);

void argValid(char *argv);
void checkArgs(int argc, char *argv[], short replace, char **libName, char **libNoExt);


// tools-general
int isNum(char c);
int fCharOrNum(char c);
int firstChar(char cc);
int ckFlag(char *word, char flags[][7]);

void perr(char *msg);
void pout(int qtt, ...);
void clearSpace(FILE *file);
void ckChar(char *word, char *blocked);
void fileChar(char *_cc, char *_cf, FILE *origin);


// tools-to-stages
int isLibFunc(char *name);
int addSpace(FILE *origin);

void declare(char *word, short jump);
void wordsBuffer(FILE *buffer, char *word);
void saveTableElement(FILE *origin, FILE *newFile, char cc);
void refeBuffer(FILE *buffer, char *orgFunct, char *orgTable, char *refe);
void saveState(FILE **origin, FILE **newFile, char *libName, char *libNoExt, FILE *buffer);

char protectedWords(FILE *origin, FILE *newFile, char cc, short printID);

// stages
void cleanupWrite(void);
void startProcess(FILE **origin, FILE **newFile, char *libName, char *libNoExt);

static void stage_01_define(FILE *origin, FILE *newFile, char *libNoExt);
static void stage_02_spaces(FILE *origin, FILE *newFile);
static void stage_03_lualib(FILE *origin, FILE *newFile);
static void stage_04_prefix(FILE *origin, FILE *newFile);
static void stage_05_compct(FILE *origin, FILE *newFile, char *LibNoExt);


#endif
