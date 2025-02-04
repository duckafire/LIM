#ifndef ARGS_PRINT_HELP
#define ARGS_PRINT_HELP

#include <stdio.h>
#include <stdlib.h>

#define MESSAGE(str) \
	puts(str);       \
	exit(0)

void show_help_messages(char *str);

static void help_with_arg(char *arg);
static void help_flag(char *flag);

#endif
