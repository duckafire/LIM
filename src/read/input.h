#ifndef lim_get_name
#define lim_get_name

#define IS_CHR(a) ((a >= 'a' && a <= 'z') || (a >= 'A' && a <= 'Z') || a == '_')
#define IS_NUM(a) (a >= '0' && a <= '9')
#define IS_SPC(a) (a == ' ' || a == '\n' || a == '\t')
#define IS_BIN(a) (a < ' ' || a == 127)

#include <stdbool.h>

static char c;

bool getName(char c, bool *fr, bool *fc);
bool getNum(char c);
bool getSpace(char c, bool fr, bool *lf);
void getSpecial(char c);

static void clearComment(bool isBLock);
static void saveString(char signal);
static bool saveDoubleSignal(char signal);

#endif
