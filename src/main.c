#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"

int main(int argc, char *argv[]){
	int flag = getFlags(argc, argv);
	if((flag == 1 || flag == 2) && argc > 2) perr("Argument overflow");
	messages(flag);

	char *origin  = argv[1];
	char *libName = checkArgs(argc, argv, flag);
	
	if(flag == 3) origin = argv[2];
	
	argValid(origin );
	argValid(libName);

	if((flag != 3 && argc > 3) || (flag == 3 && argc > 4)) perr("Argument overflow");
	free(libName);
	pout(1, "Work in progress");
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

static void messages(int flag){
	// none
	if(flag == 0){
		pout(2, 
			"[ LIM - Lua lIbrary coMpactor - https://github.com/duckafire/LIM ]",
			"Try: \"lim -h\""
		);
	}

	// version
	if(flag == 1){
		char msg[18 + VERSION_LEN] = "[ LIM - v";
		
		strcat(msg, VERSION_CUR);
		strcat(msg, " - MIT ]");
		
		pout(1, msg);
	}

	// help
	if(flag == 2){
		pout(23, 
			"[ LIM - Infomations ]",
			" ",
			"[!] Flags [!]",
			"* -v : Print the running version and the current license.",
			"* -h : Print all informations about LIM.",
			"* -r : Force the replacement of an already existig library,",
			"       if [libName] equals its name.",
			" ",
			"[!] Structure [!]",
			"* \"lim [-v ; -h]\"",
			"* \"lim [-r] <origin>.lua [libName]\"",
			" ",
			"[!] Rules [!]",
			"1. Functions declared with \"local function\", that start in",
			"   the beginning of the line, they will be added to the",
			"   library. E.g.: local function name() -> lib.name=function()",
			"2. Variables and tables declared with \"local\" (or \"_G.\"),",
			"   that start in the beginning of the line, they will not be",
			"   compacted.",
			"3. All words (except the reseved) prefixed by tabulations or",
			"   space, they will be compacted.",
			" ",
			"[ LIM - Infomations ]"
		);
	}
}
