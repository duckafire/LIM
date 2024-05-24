#ifndef __STARTED_TIN
#define __STARTED_TIN

#define FLAGS_QTT 4
#define VERSION_CUR "0.0.0"
#define VERSION_LEN 5

#define ID0 "@0" // library function
#define ID1 "@1" // "local" or global (_G) variable (or table)
#define ID2 "@2" // metamethods
#define ID3 "@3" // reserved functions ("basic")
#define ID4 "@4" // reserved table, with its function (only if it is valid)
#define ID5 "@5" // strings

#include <stdio.h>

// main.c
static void messages(int flag);
void copyOrigin(void);
void cleanup(void);

// arg.c
int getFlags(int argc, char *argv[]);
char *checkArgs(int argc, char *argv[], short replace);
void argValid(char *argv);

// tools
void perr(char *msg);
void pout(int qtt, ...);
void ckChar(char *word, char *blocked);
int ckFlag(char *word, char flags[][7]);
void fileChar(char *_cc, char *_cf, FILE *origin);
void clearSpace(FILE *file);
int firstChar(char cc);
void saveState(FILE **origin, FILE **newFile, char *libName);
void jumpToID(FILE *origin, FILE *newFile);

// write
void startProcess(FILE **origin, char *libName);
static void stage_01_define(FILE *origin, FILE *newFile, char *libName);
static void stage_02_lualib(FILE *origin, FILE *newFile);
static void stage_03_spaces(FILE *origin, FILE *newFile, FILE **newAdress, char *fileName);
static void stage_04_cmpact(FILE *origin, FILE *newFile);

#endif
