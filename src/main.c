#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"

FILE *origin, *newFile;    
char *libName;

int main(int argc, char *argv[]){
	// CHECK ALL ARGUMENTS
	int flag = getFlags(argc, argv);
	if((flag == 1 || flag == 2) && argc > 2) perr("Argument overflow");
	messages(flag);

	char *oriName;    
    oriName = argv[1];
	libName = checkArgs(argc, argv, flag);
	
	if(flag == 3) oriName = argv[2];
	
	argValid(oriName);
	argValid(libName);

	if((flag != 3 && argc > 3) || (flag == 3 && argc > 4)) perr("Argument overflow");
	
	// START PROCESS
    atexit(cleanup);

	origin = fopen(oriName, "r");
	if(origin == NULL) perr("The <origin> specified not exist");
    copyOrigin();

	newFile = fopen(libName, "r");
	if(newFile != NULL && flag != 3) perr("Already exist a \"limfile\" with name");
	
	startProcess(&origin, libName);

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
		pout(27, 
			"[ LIM - Infomations ]",
			" ",
			"[!] Flags [!]",
			"* -v : Print the running version and the current license.",
			"* -h : Print informations about LIM.",
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
			"4. Do not create strings with '[[  ]]' ",
            "5. Do not ise '@' in code (reserved words). ",
            "6. Variable and table names cannot finish with two numbers",
            "   followed (e.g.: `car12` -> car12_).",
            " ",
			"[ LIM - Infomations ]"
		);
	}
}

void copyOrigin(void){
    // trash not deleted
    remove(".limfile");

    // new file
    FILE *newOrigin;
    newOrigin = fopen(".limfile", "w");

    // copy file
    char cc;
    while((cc = fgetc(origin)) != EOF) fputc(cc, newOrigin);

    // close "font"
    fclose(origin);
    fclose(newOrigin);

    // open new file to read
    origin = fopen(".limfile", "r");
}

void cleanup(void){
	fclose(origin);
	fclose(newFile);
	free(libName);
    //remove(".limfile");
}

