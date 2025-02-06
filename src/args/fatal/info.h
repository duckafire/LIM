#ifndef ARGS_PRINT_FLAGS_IN_DEFINE
#define ARGS_PRINT_FLAGS_IN_DEFINE

#include <stdio.h>
#include <stdlib.h>

#define LIM_INFO(str, ...)      \
	printf(str, ##__VA_ARGS__); \
	exit(0)

#define no_arguments_to_lim \
	LIM_INFO("[LIM] - Lua Library Compactor\n\nTry: `lim --help`\n")

#define print_current_version(v) \
	LIM_INFO("%s\n\n", v)

#endif
