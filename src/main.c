#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"

int main(int argc, char *argv[]){
	int flag = getFlags(argc, argv);

	// none
	if(flag == 0){
		system("echo [ LIM - Lua lIbrary coMpactor - https://github.com/duckafire/LIM ]");
		return 0;
	}

	if(flag >= 1 && flag <= 2 && argc > 2) perr("Argument overflow");

	// version
	if(flag == 1){
		system("echo [ LIM - v0.2.1 - MIT ]");
		return 0;
	}

	// help
	if(flag == 2){
		// message (use stdout)
		return 0;
	}

	/*
	// tin <origin>.lua <libName>
	FILE *origin, *newFile;
	
	origin = fopen(argv[1], "r");
	if(origin == NULL) perr("The file (#1) specified not exist");

	// tin <origin>.lua <libName>
	newFile = fopen(strcat(argv[2], ".tin"), "r");
	if(newFile != NULL && id != 3) perr("The file (#2) specified already exist");

	fclose(newFile);
	newFile = fopen(argv[2], "w");
	
	// start build
	printInFile(origin, newFile, argv[2]);

	// program end
	fclose(origin);
	fclose(newFile);
	*/
	return 0;
}
