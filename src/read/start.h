#ifndef lim_start
#define lim_start

#include <stdio.h>
#include "dstr.h"

extern dstring gp_curWord;
extern FILE *gf_origin;
extern FILE *gf_buffer;

void startProcess(void);
static void getContent(void);

#endif
