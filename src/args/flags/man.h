#ifndef ARGS_RULES_FLAG_MAN
#define ARGS_RULES_FLAG_MAN

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

bool flag_cmp(char *arg, char *f, char *flag);
void message(char n, ...);

#define MESSAGE(...)               \
	message(0, __VA_ARGS__, NULL); \
	exit(0)

#endif
