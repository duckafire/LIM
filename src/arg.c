#include <stdlib.h>
#include <string.h>

#include "defs.h"

float checkArgs(int argc, char *argv[]){
	char flags[FLAGS_QTT][7] = {"--help", "--repl"};
	char blocked[10] = "/?*|>\"<>\\";

	// stage 1: quantity
	if(argc < 2) perr("Try: \"tin --help\"");
	if(argc > 4) perr("Argument overflow (max: 3)");

	// stage 2: flag or build
	if(argc == 2){
		if(ckFlag(argv[1], flags)){
			if(strcmp(argv[1], flags[0]) == 0) return 1.0; // tin --help

			perr("Incorrect use of flag. Try: \"tin --help\"");
		}
		perr("Name to new file not specified");
	}

	// stage 3: files name syntax
	for(int i = 1; i < 3; i++) ckChar(argv[i], blocked);
	
	if(strchr(argv[1], '.') == NULL) perr("File extension not specified");
	if(strcmp(strchr(argv[1], '.'), ".lua") != 0) perr("File extension not supported. Only \"LUA\"");

	if(ckFlag(argv[2], flags)) perr("Invalid use of flag. Try: \"tin <origin>.lua <libName> [flag]\"");
	
	// stage 4: optional flag
	if(argc == 4){
		if(strcmp(argv[3], flags[1]) == 0) return 3.0; // tin <file>.lua <libName> [--repl]

		perr("Invalid use of \"--repl\". Try: \"tin <origin>.lua <libName> [flag]\"");
	}
	
	return 2.0; // tin <file>.lua <libName>
}
