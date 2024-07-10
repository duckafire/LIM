#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"

FILE *origin = NULL, *newFile = NULL;
char *libName = NULL, *libNoExt = NULL;

int main(int argc, char *argv[]){
	// ADD TO CLEANUP
	atexit(cleanupMain);

	// CHECK ALL ARGUMENTS
	int flag = getFlags(argc, argv);
	if((flag == 1 || flag == 2) && argc > 2) perr("Argument overflow");
	messages(flag);

	char *oriName;
	oriName = argv[1];
	checkArgs(argc, argv, flag, &libName, &libNoExt);

	if(flag == 3) oriName = argv[2];

	argValid(oriName);
	argValid(libName);

	if((flag != 3 && argc > 3) || (flag == 3 && argc > 4)) perr("Argument overflow");

	// START PROCESS
	origin = fopen(oriName, "r");
	if(origin == NULL) perr("The <origin> specified not exist");
	copyOrigin();

	newFile = fopen(libName, "r");
	if(newFile != NULL && flag != 3) perr("Already exist a \"limfile\" with name");
	if(newFile != NULL) fclose(newFile);

	startProcess(&origin, &newFile, libName, libNoExt);

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
		pout(32,
			"[ LIM - Infomations ]",
			" ",
			"[!] Flags [!]",
			"* -v : Print the version and the license.",
			"* -h : Print 'these' informations.",
			"* -r : Replace the file, case it already exists.",
			" ",
			"[!] Structure [!]",
			"* \"lim [-v|-h]\"",
			"* \"lim [-r] <origin>.lua [<libName>]\"",
			" ",
			"[!] Rules [!]",
			"1. Functions with name prefixed by \"LIB_\" (in declaration and",
			"   calls) will be add to library table.",
			"2. Variables and tables with the prefix \"local\" (or \"_G.\")",
			"   declarated in the beginning line will not be compacted.",
			"3. Do not create strings with '[[  ]]' ",
			"4. Do not use '@' in code (reserved character). ",
			"5. Do not finish functions, variables and tables names with two",
			"   (or more) numbers (like: var12).",
			" ",
			"[!] Known bugs/problems:",
			"1. Sometimes, trash is generated close to end of file (they",
			"   shoulds removed manually).",
			"2. Files path are not supported, only the name of file, that is,",
			"   the console must be in the directory of the file.",
			"3. Hexadecimal numbers are breaked by compaction process.",
			" ",
			"[!] Full documentation [!]",
			"* https://github.com/duckafire/LIM/tree/main/info",
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

void cleanupMain(void){
	if(origin   != NULL) fclose(origin);
	if(newFile  != NULL) fclose(newFile);
	if(libName  != NULL) free(libName);
	if(libNoExt != NULL) free(libNoExt);
	remove(".limfile");
	cleanupWrite();
}

