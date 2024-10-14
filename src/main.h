#ifndef lim_main
#define lim_main

#define ARRAY_LEN(a) (sizeof(a) / sizeof(a[0]))

#include <stdbool.h>

extern bool g_verbose;
extern bool g_replace;

extern char *gp_nameOrg;
extern char *gp_nameDst;

static short strcmp2(char *str, char *v0, char *v1);

#endif
