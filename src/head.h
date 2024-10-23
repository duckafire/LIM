#ifndef lim_root
#define lim_root

#define LIM_VERSION "v1.0.0-alpha"
#define ARRAY_LEN(a) (sizeof(a) / sizeof(a[0]))

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>

extern bool g_verbose;
extern bool g_replace;
extern char *gp_nameOrg;
extern char *gp_nameDst;

bool strcmp2(char *str, char *v0, char *v1);
FILE* copyFile(FILE *org, char *dstName);
static void cleanup(void);

#endif
