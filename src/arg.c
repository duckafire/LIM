#include <stdlib.h>
#include <string.h>

#include "defs.h"

int checkArgs(int argc, char *argv[]){
	// 0: tin
	// 1: tin --help
	// 2: tin origin.lua newFile
	// 3: tin origin.lua newFile --repl

	// sequence index
	int id = 0;

	// program flags
	char flags[2][7] = {"--help", "--repl"};

	// search invalid arguments
	int max = argc;
	if(argc > 4) max = 4;
		
	int jbreak = 0;

	for(int i = 1; i < max; i++){
		// check parameters
		for(int j = 0; j < 2; j++){
			if(argv[i], flags[j]){
				// any flag
				id++;
				jbreak = 1;

				// it is "--help"
				if(id == 1){
					jbreak = 2;
					if(argc > 2) system("echo [tin] Some arguments were ignored.");
				}

				break;
			}
		}
		
		// flag finded
		if(jbreak == 1) continue; else if (jbreak == 2) break;
		
		// files
		if(i == 2 || (i == 1 && strcmp(strchr(argv[i], '.'), ".lua") == 0)){
			id++;
			continue;
		}else if(i == 1){
			system("echo [tin] Extension not supported. Only \"LUA\".");
			exit(1);
		}
		
		// the argument #3 is optional
		if(i == 3) break;

		// madatory arguments not specified
		system("echo [tin] Argument invalid. Try \"tin --help\".");
		exit(1);
	}

	if(max < argc) system("echo [tin] Some arguments were ignored.");

	// sequence
	return id;
}

