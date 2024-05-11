#ifndef __STARTED_TIN
#define __STARTED_TIN

#define FLAGS_QTT 4
#define VERSION_CUR "0.0.0"
#define VERSION_LEN 5

#include <stdio.h>

// main.c
static void messages(int flag);

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

// write
void printInFile(FILE *origin, FILE *newFile, char *libName);
static void stage_01_define(FILE *origin, FILE *newFile, char *libName);
static void stage_02_lualib(FILE *origin, FILE *newFile);

#endif
