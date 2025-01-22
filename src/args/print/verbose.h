#ifndef ARGS_PRINT_VERBOSE
#define ARGS_PRINT_VERBOSE

#include <stdarg.h>
#include <stdbool.h>

enum{
	V_NORMAL,
	V_TITLE,
	V_FLAG_STATUS,
	V_STARTING,
	V_CLOSING,
	V_CODE_STATUS_NUM,
	V_CODE_STATUS_STR,
	V_BREAK_PROCESS,
};

void set_verbose(bool flag);
void print_verbose(short tag, ...);
static void normal(char *text);
static void title( char *text);
static void flag_status(void);
static void starting_or_closing(bool is_start);
static void code_status(bool is_num);
static void break_process(void);

#endif
