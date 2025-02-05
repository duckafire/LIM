#ifndef ARGS_PRINT_FLAGS_IN_DEFINE
#define ARGS_PRINT_FLAGS_IN_DEFINE

#include <stdio.h>
#include <stdlib.h>
#include "../../cmake-h/version.h"

#define no_arguments_to_lim \
	puts("[LIM] - Lua Library Compactor\n\nTry: `lim --help`\n"); \
	exit(0)

#define print_current_version     \
	printf("%s\n\n", LIM_VERSION);\
	exit(0)

#endif
