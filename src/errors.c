#include <stdlib.h>

#include "global.h"
#include "flags.h"
#include "errors.h"

void repeatFlag(short pos, char *arg){
	pParag(0, "error", NULL);
	exit(E_REPEAT_FLAG);
}

void unexpectedFlag(short pos, char *arg){
	pParag(0, "error", NULL);
	exit(E_UNEXPECTED_FLAG);
}

void argExpected(char *_r, char *requester){
	pParag(0, "error", NULL);
	exit(E_ARG_EXPECTED);
}

void invalidFlag(char *arg){
	pParag(0, "error", NULL);
	exit(E_INVALID_FLAG);
}

void filesNamesOverflow(void){
	pParag(0, "error", NULL);
	exit(E_FILES_NAMES_OVERFLOW);
}
