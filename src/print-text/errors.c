#include <stdio.h>
#include <stdlib.h>
#include "../sub-heads.h"

void repeatFlag(char *arg, short pos){
	fprintf(stdout, E_MSG_FORMAT_0, E_MSG_1, arg, pos);
	exit(1);
}

void unexpectedFlag(char *arg, short pos){
	fprintf(stdout, E_MSG_FORMAT_0, E_MSG_2, arg, pos);
	exit(2);
}

void argExpected(char *_r, char *requester){
	fprintf(stdout, E_MSG_FORMAT_1, E_MSG_3, requester);
	exit(3);
}

void invalidFlag(char *arg, short pos){
	fprintf(stdout, E_MSG_FORMAT_0, E_MSG_4, arg, pos);
	exit(4);
}

void filesNamesOverflow(void){
	fprintf(stdout, E_MSG_FORMAT_2, E_MSG_5);
	exit(5);
}

void nameNotSpecified(void){
	fprintf(stdout, E_MSG_FORMAT_2, E_MSG_6);
	exit(6);
}

void nonExistentFile(char *name){
	fprintf(stdout, E_MSG_FORMAT_1, E_MSG_7, name);
	exit(7);
}

void fileAlreadyExistent(char *name){
	fprintf(stdout, E_MSG_FORMAT_1, E_MSG_8, name);
	exit(8);
}
