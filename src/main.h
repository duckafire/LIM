#ifndef lim_main
#define lim_main

#define LIM_VERSION "v1.0.0-alpha"
#define ARRAY_LEN(a) (sizeof(a) / sizeof(a[0]))

#include <stdbool.h>
#include <stdarg.h>

#include "queuePointer.h"

extern bool g_verbose;
extern bool g_replace;
extern char *gp_nameOrg;
extern Queue *gp_nameDst;

bool strcmp2(char *str, char *v0, char *v1);
static void cleanup(void);

#endif
