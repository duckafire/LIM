#ifndef ARGS_PRINT_VERBOSE
#define ARGS_PRINT_VERBOSE

#include <stdarg.h>
#include <stdbool.h>

typedef enum{
	V_FLAGS_STATUS,
	V_HEADER_STATUS,
	V_START_PROCESS,
	V_CLOSE_PROCESS,
	V_CONST_FOUND,
	V_IDENT_FOUND,
	V_INSERTING,
	V_WARNING,
	V_NEW_THING,
	V_LOST_THING,
	V_END_THING,
}VERBOSE_TAG;

void pverbose(VERBOSE_TAG tag, ...);
static char* bool_to_str(bool val);
static void was_const(void);
static void flags_status(void);
static void header_lim_status(void);
static void start_or_close_process(bool is_start);
static void const_found(void);
static void ident_found(void);
static void inserting(void);
static void warning(void);
static void thing(short code);

#endif
