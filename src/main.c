#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"

int main(int argc, char *argv[]){
	// 1: tin --help
	// 2: tin <file>.lua <newFileName>
	// 3: tin <file>.lua <newFileName> --repl
	
	float id = checkArgs(argc, argv);
	
	if(id == 1.0){
		system("echo # ==================================================");
		system("echo # [ tin - v0.1.1 - https://github.com/duckafire/TIN");
		system("echo # ");
		system("echo # It is a small terminal program, created to compact");
		system("echo # files (libraries) of the TinyLibrary.");
		system("echo # ");
		system("echo # [!] Comands");
		system("echo # ");
		system("echo # \"tin --help\"");
		system("echo # \"tin <origin>.lua <libName>\"");
		system("echo # \"tin <origin>.lua <libName> --repl\"");
		system("echo # ==================================================");
		return 0;
	}
	
	FILE *origin, *newFile;

	// origin
	origin = fopen(argv[1], "r");
	if(origin == NULL) perr("The file (#1) specified not exist");

	// newFileName
	newFile = fopen(strcat(argv[2], ".tin"), "r");
	if(newFile != NULL && id != 3) perr("The file (#2) specified already exist");

	fclose(newFile);
	newFile = fopen(argv[2], "w");
	
	// build
	printInFile(origin, newFile, argv[2]);

	// end
	fclose(origin);
	fclose(newFile);
	return 0;
}
