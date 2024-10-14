#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "flags.h"

void welcome(void){
	pParag(0, "Welcome message", "dsadad", "sadasd", NULL);
}

void version(void){
	pParag(0, LIM_VERSION, NULL);
}

void manual(void){
	pParag(0, "Manual", NULL);
	exit(0);
}

#include <stdio.h>
void help(char *flag){
	if(strcmp2(flag, F_VERSION)){
	
	//} else if()...
	}else{ // NULL
		pParag(0, "Default message", NULL);
	}

	exit(0);
}

void verbose(void){
	if(!g_verbose) return;

	static short id = -1;
	id++;

	if(id == 0){
		pParag(0, "lorem ipsum", NULL);
		return;
	}
	//...
}

