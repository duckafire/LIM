#ifndef __STARTED_TIN
#define __STARTED_TIN

#define FLAGS_QTT 4

#include <stdio.h>

// arg.c
int getFlags(int argc, char *argv[]);
char *checkArgs(int argc, char *argv[], short replace);

// tools
void perr(char *msg);
void ckChar(char *word, char *blocked);
int ckFlag(char *word, char flags[][7]);
void fileChar(char *_cc, char *_cf, FILE *origin);

// write
void printInFile(FILE *origin, FILE *newFile, char *libName);
static void stage_01_newName(FILE *origin, FILE *newFile, char *libName);
// static void stage_02_noCommt(FILE *origin, FILE *newFile);
// static void stage_03_packLib(FILE *origin, FILE *newFile);
// static void stage_04_compact(FILE *origin, FILE *newFile);

#endif
