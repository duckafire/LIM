#ifndef __STARTED_TIN
#define __STARTED_TIN

#define FLAGS_QTT 4
#define VERSION_CUR "0.0.0"
#define VERSION_LEN 5

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
long fileLenght(FILE *file);
void *saveState(FILE *new, FILE **tmp);

// write
void startProcess(FILE *origin, char *libName);
static void stage_01_define(FILE *origin, FILE *newFile, char *libName);
static void stage_02_lualib(FILE *origin, FILE *newFile);
static void stage_03_spaces(FILE *origin, FILE *newFile, FILE **newAdress, char *fileName);
static void stage_04_cmpact(FILE *origin, FILE *newFile);

#endif
