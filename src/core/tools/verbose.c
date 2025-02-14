#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "lim-global-variables.h"
#include "verbose.h"

static va_list content;
static bool const_chain = false;
static short const_quant = 0;

#define ITEM(s)  "  " s ": %s\n"
#define STR va_arg(content, char*)
#define INT va_arg(content, int)

#define NORMAL   "[lim] "
#define START(s) printf(">lim> Starting: '%s'...\n", s)
#define CLOSE(s) printf("<lim< Closing: '%s'...\n", s)
#define CONST_ID "|lim| "
#define INSERT   "{lim} "
#define WARNING  "[LIM] "
#define THING    "(lim) "

void pverbose(VERBOSE_TAG tag, ...){
	va_start(content, tag);

	switch(tag){
		case V_FLAGS_STATUS:  flags_status(); break;
		case V_HEADER_STATUS: header_lim_status(); break;
		case V_START_PROCESS: start_or_close_process(true); break;
		case V_CLOSE_PROCESS: start_or_close_process(false); break;
		case V_CONST_FOUND:   const_found(); break;
		case V_IDENT_FOUND:   ident_found(); break;
		case V_INSERTING:     inserting(); break;
		case V_WARNING:       warning(); break;
		case V_NEW_THING:     thing(0); break;
		case V_LOST_THING:    thing(1); break;
		case V_END_THING:     thing(2); break;
	}

	va_end(content);
}

static char* bool_to_str(bool val){
	if(val)
		return "true";

	return "false";
}

static void was_const(void){
	if(!const_chain)
		return;

	const_chain = false;
	const_quant = 0;
	putchar('\n');
}

static void flags_status(void){
	puts(NORMAL "Flags status:");
	printf(ITEM("Verbose mode"),         bool_to_str(lim.flags.verbose));
	printf(ITEM("Replace destine file"), bool_to_str(lim.flags.replace));
	printf(ITEM("Search 'header.lim'"),  bool_to_str(lim.flags.header_file));
	printf(ITEM("Library table name"),   lim.flags.lib_name);
}

static void header_lim_status(void){
	printf(NORMAL "'header.lim' status: %d\n", INT);
	printf("  Partitions status: %s\n", STR);
}

static void start_or_close_process(bool is_start){
	was_const();

	char *process;
	process = STR;

	if(is_start)
		START(process);
	else
		CLOSE(process);
}

static void const_found(void){
	if(!const_chain){
		static const short title_len = strlen(CONST_ID "Constants found: ");
		printf(CONST_ID "Constants found: ");

		const_chain = true;
		const_quant = title_len;
	}

	char *_const = STR;

	const_quant += strlen(_const);
	if(const_quant > 70){
		const_quant = 0;
		printf("\n  ");
	}

	printf("%s ", _const);
}

static void ident_found(void){
	was_const();

	const char *from[3] = {"Lua Standard", "'header.lim'", "input file"};

	printf(CONST_ID "Identifier found (%s):\n", from[INT]);
	printf(ITEM("ident"), STR);

	const char *table_key = STR;
	if(table_key != NULL)
		printf(ITEM("table key"), table_key);
}

static void inserting(void){
	was_const();

	const char *msg;
	msg = STR;

	if(msg != NULL)
		printf(INSERT "Inserting: %s...\n", msg);
}

static void warning(void){
	was_const();

	char *cur;
	printf(WARNING);

	while( (cur = STR) != NULL )
		printf("%s ", cur);

	putchar('\n');
}

static void thing(short code){
	was_const();

	char *def_msg = (code == 0) ? "New" : ((code == 1) ? "Lost" : "End") ;
	short quant = (short)INT;

	printf(THING "%s (#%d): %s\n", def_msg, quant, STR);
}
