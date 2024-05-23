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
	
	//startProcess(origin, libName);

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
		pout(25, 
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
            " ",
			"[ LIM - Infomations ]"
		);
	}
}

void copyOrigin(void){
    // buffer to store file content
    long size = fileLenght(origin);

    char transfer[size];
    memset(transfer, '\0', size);

    perr("BETA");
    // get content
    fscanf(origin, "%700s", transfer);
    //fgets(transfer, size, origin);

    FILE *temp;
    temp = fopen("car.txt", "w");
    fprintf(temp, "%d - %d", strlen(transfer), size);
    fclose(temp);
    
    // close "font" file
    fclose(origin);

    // create a new file, copy content to it and close it
    origin = fopen(".limfile", "w");
    fputs(transfer, origin);
    fclose(origin);

    // reopen it in read mode
    origin = fopen(".limfile", "r");
}

void cleanup(void){
	fclose(origin);
	fclose(newFile);
	free(libName);
    //remove(".limfile");
}

