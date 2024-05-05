#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"

int main(int argc, char *argv[]){
	int sequence = checkArgs(argc, argv);
	FILE *origin, *newFile;
	
	if(sequence == 1){
		system("echo ");
		system("echo [ tin - 0.1.0 - MIT - https://github.com/duckafire/TIN ]");
		system("echo ");
		system("echo ~ tin is small terminal program, created to compact");
		system("echo files of the TintLibrary.");
		system("echo ");
		system("echo [ Command ]");
		system("echo ");
		system("echo tin --help");
		system("echo tin <origin>.lua <newFile>");
		system("echo tin <origin>.lua <newFile> --repl");
		system("echo ");
	}

	if(sequence != 1){
		origin = fopen(argv[1], "r");
		if(origin == NULL){
			system("echo [tin] File (#1) not founded.");
			exit(1);
		}

		newFile = fopen(strcat(argv[2], ".lua"), "w");
		if(newFile != NULL && sequence != 3){
			system("echo [tin] The file specified (#2) already exist.");
			exit(1);
		}
	
	}

	fclose(origin );
	fclose(newFile);
	return 0;
}
