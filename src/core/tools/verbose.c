#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "lim-global-variables.h"
#include "verbose.h"

static va_list content;


// #define LIM_ERROR "<LIM>" (./error.h)
#define LIM_NORMAL   "[lim] "
#define LIM_TITLE    "[LIM] "
#define LIM_F_STATUS "{lim} "
#define LIM_STARTING ">lim> "
#define LIM_CLOSING  "<lim< "
#define LIM_C_STATUS "(lim) "
#define LIM_BREAK_P  "|lim| "


#define START(lim) \
	printf(lim);   \
	putchar(toupper(text[0]))

#define FOR \
	for(short for_i = 1; text[for_i] != '\0'; for_i++)

#define END \
	puts(".")

void print_verbose(short tag, ...){
	if(!lim.flags.verbose)
		return;

	va_start(content, tag);

	switch(tag){
		case V_NORMAL:          normal(va_arg(content, char*) ); break;
		case V_TITLE:           title( va_arg(content, char*) ); break;
		case V_FLAG_STATUS:     flag_status();                   break;
		case V_STARTING:        starting_or_closing(true);       break;
		case V_CLOSING:         starting_or_closing(false);      break;
		case V_CODE_STATUS_NUM: code_status(true);               break;
		case V_CODE_STATUS_STR: code_status(false);              break;
		case V_BREAK_PROCESS:   break_process();                 break;
	}

	va_end(content);
}

static void normal(char *text){
	START(LIM_NORMAL);

	FOR printf("%c", text[ for_i ]);

	END;
}

static void title(char *text){
	putchar('\n');

	START(LIM_TITLE);

	FOR printf("%c", toupper(text[ for_i ]));

	END;
}

static void flag_status(void){
	int status;
	char *f, *flag, text_status[4];
	
	f      = va_arg(content, char*);
	flag   = va_arg(content, char*);
	status = va_arg(content, int);

	if(status)
		strcpy(text_status, "ON");
	else
		strcpy(text_status, "OFF");

	printf(LIM_F_STATUS "%s (%s): %s\n", flag, f, text_status);
}

static void starting_or_closing(bool is_start){
	if(is_start)
		printf(LIM_STARTING "Starting: ");
	else
		printf(LIM_CLOSING "Closing: ");

	char *cur;

	while( (cur = va_arg(content, char*)) != NULL )
		printf(" %s;", cur);

	putchar('\n');
}

static void code_status(bool is_num){
	char *identifier, status[10];

	identifier = va_arg(content, char*);

	if(is_num)
		sprintf(status, "%d", (va_arg(content, int)) );
	else
		strcpy(status, va_arg(content, char*));

	printf(LIM_C_STATUS "Output status from \"%s\": %s\n", identifier, status);
}

static void break_process(void){
	printf(LIM_BREAK_P "Compaction break in Stage %d.\n\n", va_arg(content, int));
}
