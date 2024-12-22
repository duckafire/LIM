#ifndef INTERFACE_ARGS_FLAG_MAN
#define INTERFACE_ARGS_FLAG_MAN

#include <stdlib.h>
#include <stdbool.h>

bool flag_cmp(char *arg, char *f, char *flag);
void message(char n, ...);

#define MESSAGE(...)               \
	message(0, __VA_ARGS__, NULL); \
	exit(0)

#endif
