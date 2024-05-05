#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"

int main(int argc, char *argv[]){
	checkArgs(argc, argv);
	system("echo Hello World!");
	return 0;
}
