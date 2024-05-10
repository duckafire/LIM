#include <stdlib.h>
#include <string.h>

#include "defs.h"

int getFlags(int argc, char *argv[]){
	// none, null, -v, -h, -r
	//   -1,    0,   1, 2,  3
	
	if(argc == 1) return 0;

	if(argv[1][0] == '-'){
		if(strlen(argv[1]) == 2){
			char flags[3] = {'v', 'h', 'r'};
			for(int i = 0; i <= 3; i++) if(argv[1][1] == flags[i]) return i + 1;
		}
		perr("Invalid flag");
	}

	return -1;
}

char *checkArgs(int argc, char *argv[], short replace){
	// origin; libName (+1); quantity of arguments expected - all with "-r"
	short argID = 1;
	if(replace == 3) argID = 2; // without "-r"

	// get origin
	if(argc == argID) perr("<origin> not specified");

	if(strchr(argv[argID], '.') != NULL){
		if(strcmp(strchr(argv[argID], '.'), ".lua") != 0) perr("Extension not supported. Only \"LUA\"");
	}else{
		perr("<origin> extension not specified (.lua)");
	}

	// libName: specified; not specified
	int add = 8;
	if(argc == argID + 1){
		argID--;
		add = 4;
	}

	int len = strlen(argv[argID + 1]) + add;
	char *name = malloc(len);
	memset(name, '\0', len);

	strcpy(name, argv[argID + 1]);
	if(add == 4) name[len - 8] = '\0';
	strcat(name, ".limfile");

	return name;
}

void argValid(char *argv){
	char blocked[10] = "/\\:*?\"<>";
	
	for(int i = 0; i < strlen(argv); i++){
		for(int j = 0; j < strlen(blocked); j++){
			if(argv[i] == blocked[j]) perr("Invalid character finded.");
		}
	}
}
