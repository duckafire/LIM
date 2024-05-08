#ifndef __STARTED_TIN
#define __STARTED_TIN

#define FLAGS_QTT 2

#include <stdio.h>

// arg.c
float checkArgs(int argc, char *argv[]);

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
