#ifndef lim_read
#define lim_read

#include <stdio.h>
#include <stdbool.h>

// DSTR
#define UNINT unsigned int

void dstr_init(void);
void dstr_addc(char c);
void dstr_fputs(void);
void dstr_end(void);

// INPUT
#define IS_CHR(a) ((a >= 'a' && a <= 'z') || (a >= 'A' && a <= 'Z') || a == '_')
#define IS_NUM(a) (a >= '0' && a <= '9')
#define IS_SPC(a) (a == ' ' || a == '\n' || a == '\t')
#define IS_BIN(a) (a < ' ' || a == 127)

static char c;

bool getName(char c, bool *fr, bool *fc);
bool getNum(char c);
bool getSpace(char c, bool fr, bool *lf);
void getSpecial(char c);

static void clearComment(bool isBLock);
static void saveString(char signal);
static bool saveDoubleSignal(char signal);

// START
extern FILE *gf_origin;
extern FILE *gf_buffer;

void startProcess(void);
static void getContent(void);

#endif