#include <stdlib.h>
#include <string.h>

#include "defs.h"

void checkArgs(int argc, char *argv[]){
	char flags[FLAGS_QTT][7] = {"--help", "--repl"};
	char blocked[10] = "/?*|>\"<>\\";
	
	system("echo 1");
	if(argc < 2) perr("None parameters of flags specified.");
	if(argc > 4) perr("Argument overflow (max: 3).");

	system("echo 2");
	if(argc == 2){
		if(ckFlag(argv[1], flags)){
			if(strcmp(argv[1], flags[0]) == 0){
				system("echo HELP MESSAGE");
			}else{
				perr("Incorrect use of flag. Try: \"tin --help\".");
			}
		}else{
			perr("Name to new file not specified");
		}
	}

	system("echo 3");
	for(int i = 1; i < 3; i++) ckChar(argv[i], blocked);
	
	if(strchr(argv[1], '.') == NULL) perr("File extension not specified.");
	if(strcmp(strchr(argv[1], '.'), ".lua") != 0) perr("File extension not supported. Only \"LUA\".");

	system("echo 4");
	if(ckFlag(argv[2], flags)) perr("Invalid use of flag. Try: \"tin <origin>.lua <newFileName> [flag]\"");
	
	system("echo 5");
	if(argc == 4){
		if(strcmp(argv[3], flags[1]) != 0) perr("Invalid use of \"--repl\". Try: \"tin <origin>.lua <newFileName> [flag]\"");
	}
	
	system("echo 6");
}
