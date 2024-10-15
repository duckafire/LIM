#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "flags.h"

void welcome(void){
	pMessage(0, "Welcome message", NULL);
	exit(0);
}

void version(void){
	pMessage(0, "Version message", NULL);
	exit(0);
}

void helpList(void){
	pMessage(0, "Help-list message", NULL);
	exit(0);
}

void help(char *flag){
	if(strcmp2(flag, F_VERSION))
		pMessage(0, "Help message(version)", NULL);
	else if(strcmp2(flag, F_H_LIST ))
		pMessage(0, "Help message(help-list)", NULL);
	else if(strcmp2(flag, F_HELP   ))
		pMessage(0, "Help message(help)", NULL);
	else if(strcmp2(flag, F_VERBOSE))
		pMessage(0, "Help message(verbose)", NULL);
	else if(strcmp2(flag, F_NAME   ))
		pMessage(0, "Help message(name)", NULL);
	else if(strcmp2(flag, F_REPLACE))
		pMessage(0, "Help message(replace)", NULL);
	else if(strcmp2(flag, F_LICENSE))
		pMessage(0, "Help message(license)", NULL);
	else
		pMessage(0, "Help message (all)", NULL);

	exit(0);
}

void verbose(void){
	if(!g_verbose) return;

	static short id = -1;
	id++;

	if(id == 0){
		pMessage(0, "Started", NULL);
		return;
	}
	//...
}

void license(void){
	pMessage(0, "License message", NULL);
	exit(0);
}
