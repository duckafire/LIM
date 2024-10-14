#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "flags.h"

void pParag(char **parag){
	for(short i = 0; i < ARRAY_LEN(parag); i++)
		fprintf(stdout, "%s\n", parag[i]);

	exit(0);
}

void welcome(void){}
void version(void){}
void manual(void){}
void help(char *flag){}
void verbose(void){}
