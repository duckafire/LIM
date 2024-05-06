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

// write
void printInFile(FILE *origin, FILE *newFile);

#endif
