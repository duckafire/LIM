#ifndef lim_read
#define lim_read

#include <stdio.h>
#include <stdbool.h>

// COLLECT
#define UNINT unsigned int

void collect_init(void);
void collect_add(char c);
void collect_end(void);

// INPUT
#define IS_CHR(a) ((a >= 'a' && a <= 'z') || (a >= 'A' && a <= 'Z') || a == '_')
#define IS_NUM(a) (a >= '0' && a <= '9')
#define IS_BIN(a) (a <= ' ' || a == 127)

static char c;

bool getName(char c, bool firstChar);
bool getNum(char c);
bool getSpace(char c);
void getSpecial(char c);

static void clearComment(bool isLine);
static void saveString(char signal);
static void saveBraces(void);
static bool saveDoubleSignal(char signal);

// START
extern FILE *gf_origin;

void startProcess(void);
static void getContent(void);

#endif
