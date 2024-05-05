#ifndef __STARTED_TIN
#define __STARTED_TIN

#define FLAGS_QTT 2

void checkArgs(int argc, char *argv[]);
void perr(char *msg);
void ckChar(char *word, char *blocked);
int ckFlag(char *word, char flags[][7]);

#endif
