#include <stdlib.h>
#include <string.h>

#include "defs.h"

// char flags[FLAGS_QTT][7] = {"--help", "--rule", "--back", "--repl"};
// char blocked[10] = "/?*|>\"<>\\";

int getFlags(int argc, char *argv[]){
	// none, null, -v, -h, -r
	//   -1,    0,   1, 2,  3
	
	if(argc == 0) return 0;

	if(argv[1][0] == '-'){
		if(strlen(argv[1]) == 2){
			char flags[3] = {'v', 'h', 'r'};
			for(int i = 0; i < 3; i++) if(argv[1][1] == flags[i]) return i + 1;
		}
		perr("Invalid flag");
	}

	return -1;
}

char *checkArgs(int argc, char *argv[], short replace){
	short qtt = 2;
	if(!replace) qtt = 1; // less "-r"
			      
	// get origin
	if(argc == qtt) perr("\"origin\" not specified");

	if(strcmp(strchr(argv[1], '.'), ".lua") != 0) perr("Extension not supported. Only \"LUA\"");

	// library name not specified
	if(argc == qtt + 1){
		// len - 4 + 8 + 1
		char temp[strlen(argv[1] + 5)];

		for(int i = 0; i < strlen(argv[1]) - 4; i++) temp[i] = argv[1][i];

		strcat(temp, ".limfile");
		//return temp;
		return NULL;
	}

	return argv[2];
}
