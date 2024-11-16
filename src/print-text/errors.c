#include <stdio.h>
#include <stdlib.h>
#include "../heads.h"

void er_repeatFlag(char *arg, short pos){
	printf(E_MSG_FORMAT_0, E_MSG_1, arg, pos);
	exit(1);
}

void er_unexpectedFlag(char *arg, short pos){
	printf(E_MSG_FORMAT_0, E_MSG_2, arg, pos);
	exit(2);
}

void er_argExpected(char *_r, char *requester){
	printf(E_MSG_FORMAT_1, E_MSG_3, requester);
	exit(3);
}

void er_invalidFlag(char *arg, short pos){
	printf(E_MSG_FORMAT_0, E_MSG_4, arg, pos);
	exit(4);
}

void er_filesNamesOverflow(void){
	printf(E_MSG_FORMAT_2, E_MSG_5);
	exit(5);
}

void er_nameNotSpecified(void){
	printf(E_MSG_FORMAT_2, E_MSG_6);
	exit(6);
}

void er_nonExistentFile(char *name){
	printf(E_MSG_FORMAT_1, E_MSG_7, name);
	exit(7);
}

void er_fileAlreadyExistent(char *name){
	printf(E_MSG_FORMAT_1, E_MSG_8, name);
	exit(8);
}
