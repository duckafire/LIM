#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "../head.h"
#include "head.h"

static void message(char n, ...){
	va_list parag;
	va_start(parag, n);

	char *cur;

	while((cur = va_arg(parag, char*)) != NULL)
		fprintf(stdout, "%s\n", cur);

	fputc('\n', stdout);
	va_end(parag);
	exit(0);
}

void welcome(void){
	message(0, "Welcome message", NULL);
	exit(0);
}

void version(void){
	message(0, "Version message", NULL);
	exit(0);
}

void helpList(void){
	message(0, "Help-list message", NULL);
	exit(0);
}

void help(char *flag){
	if(strcmp2(flag, F_VERSION))
		message(0, "Help message(version)", NULL);
	else if(strcmp2(flag, F_H_LIST ))
		message(0, "Help message(help-list)", NULL);
	else if(strcmp2(flag, F_HELP   ))
		message(0, "Help message(help)", NULL);
	else if(strcmp2(flag, F_VERBOSE))
		message(0, "Help message(verbose)", NULL);
	else if(strcmp2(flag, F_NAME   ))
		message(0, "Help message(name)", NULL);
	else if(strcmp2(flag, F_REPLACE))
		message(0, "Help message(replace)", NULL);
	else if(strcmp2(flag, F_LICENSE))
		message(0, "Help message(license)", NULL);
	else
		message(0, "Help message (all)", NULL);

	exit(0);
}

void verbose(void){
	if(!g_verbose) return;

	static short id = -1;
	id++;

	if(id == 0){
		message(0, "Started", NULL);
		return;
	}
	//...
}

void license(void){
	message(0, "License message", NULL);
	exit(0);
}
