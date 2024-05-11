#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "defs.h"

void printInFile(FILE *origin, FILE *newFile, char *libName){
	// get library functions, variables and tables; remove unnecessary line feed
	stage_01_define(origin, newFile, libName);
	
	// search lua libraries and replce them by refences; remove unnecessary tabulations
	// stage_02_lualib(origin, newFile);
}

static void stage_01_define(FILE *origin, FILE *newFile, char *libName){
	char init[6], func[9], name[50], cc;
	int qtt = 0;

	memset(init, '\0',  6); // local/_G
	memset(func, '\0',  9); // function
	memset(name, '\0', 50); // function name

	// remove extension of the library name
	int len = strlen(libName) - 8;
	char *lib;

	lib = malloc(len);
	memset(lib, '\0', len);
	
	for(int i = 0; i < len; i++) lib[i] = libName[i];

	// get local/_G
	while(fscanf(origin, "%6[^\n. ]", init) != -1){
		
		if(strcmp(init, "local") == 0 || strcmp(init, "_G") == 0){
			clearSpace(origin);
			// get function reservad word
			fscanf(origin, "%8[^\n]", func);

			if(strcmp(func, "function") == 0){
				clearSpace(origin);
				// get name of the function
				fscanf(origin, "%50[^\n(]", name);

				fprintf(newFile, "%s.%s=function", lib, name);
			}else{
				fseek(origin, -strlen(func), SEEK_CUR);
				fprintf(newFile, "^%s ", init);
			}
		}else{
			fseek(origin, -strlen(init), SEEK_CUR);
		}

		qtt = 0;
		while((cc = fgetc(origin)) != '\n'){
			qtt++;
			fputc(cc, newFile);
		}
		if(qtt > 0) fputc('\n', newFile);

		memset(init, '\0',  6);
		memset(func, '\0',  9);
		memset(name, '\0', 50);
	}

	free(lib);
}

static void stage_02_lualib(FILE *origin, FILE *newFile){
	// lua library; tables from 23, before only functions (basic "pack")
	char resMain[31][15] = { "assert", "collectgarbage", "dofile", "error", "getmetatable", "ipairs", "load", "loadfile", "next", "pairs", "pcall", "print", "rawequal", "rawget", "rawlen", "require", "select", "setmetatable", "tonumber", "tostring", "type", "warn", "xpcall", "courotine", "debug", "io", "math", "os", "package", "string", "table"};
	
	// functions to lua library tables
	char resSub[9][27][15] = {
		/*courotine*/{"close", "create", "isyieldable", "resume", "running", "status", "wrap", "yield"},
		/*debug    */{"debug", "gethook", "getinfo", "getlocal", "getmetatable", "getregistry", "getupvalue", "getuservalue", "sethook", "setlocal", "setmetatable", "setupvalue", "setuservalue", "traceback", "upvalueid", "upvaluejoin"},
		/*math     */{"abs", "acos", "asin", "atan", "ceil", "cos", "deg", "exp", "floor", "fmod", "huge", "log", "max", "maxinteger", "min", "mininteger", "modf", "pi", "rad", "random", "randomseed", "sin", "sqrt", "tan", "tointeger", "type", "ult"},
		/*io       */{"clock", "date", "difftime", "execute", "exit", "getenv", "remove", "rename", "setlocale", "time", "tmpname"},
		/*math     */{"config", "cpath", "loaded", "loadlib", "path", "preload", "searchers", "searchpath"},
		/*os       */{"close", "flush", "input", "lines", "open", "output", "popen", "read", "tmpfile", "type", "write"},
		/*package  */{"char", "charpattern", "codepoint", "codes", "len", "offset"},
		/*string   */{"byte", "char", "dump", "find", "format", "gmatch", "gsub", "len", "lower", "match", "pack", "packsize", "rep", "reverse", "sub", "unpack", "upper"},
		/*table    */{"concat", "insert", "move", "pack", "remove", "sort", "unpack"}
	};

	// current character; current word; references to functions
	char cc, word[10], ref[3];
	// new word finded; index to "resMain"; index to "resSub"
	int  newWord = 0, mainID = 0, subID = 0;
	// search in file
	while((cc = fgetc(origin)) != EOF){
		// clear
		memset(word, '\0', 10);
		// check if it is a first character "valid" (a-z)
		if(newWord && (cc >= 97 || cc <= 122)){
			// word finded
			newWord = 0;
			// to compensate "cc"
			fseek(origin, -1, SEEK_CUR);
			// get word finded
			fscanf(origin, "%10[^\n.]", word);
			// clear indexes
			mainID = subID = 0;
			// compare the word with all reserved words
			for(int i = 0; i < 32; i++){ // reserved = 32
				// compare
				if(strcmp(word, resMain[i]) == 0){
					// index finded
					mainID = i;
					// check if it is a table
					if(mainID >= 22){
						// jump '.'
						fseek(origin, 1, SEEK_CUR);
						// get new word finded
						fscanf(origin, "%10[^\n(]");
						// unnecessary?
						// clearSpace(origin);
						for(int j = 0; j < 2; j++){ // sub-reserved = "2"
							// compare the new word finded with all reserved functions
							if(strcmp(word, resSub[i][j]) == 0){
								// index finded
								subID = 0;
								// end search of sub
								break;
							}
						}
					}
				// end search of main
				break; // 'i'
				}
			}
		}else{
			// it is not a valid character
			fputc(cc, newFile);
		}
		// finded end of the last word
		if(cc == ' ' || cc == '\n' || cc == '\t') newWord = 1;
		// check if a index was obtained
		if(mainID != 0){
			// defealt reference (only main)
			ref[0] = resMain[mainID][0];
			ref[1] = resMain[mainID][2];
			ref[2] = NULL;
			// reference with sub
			if(subID == 0){
				ref[0] = resMain[mainID][0];
				ref[1] = resSub[ subID ][0];
				ref[2] = resSub[ subID ][2];
			}
			// write reference (replace original word)
			for(int i = 0; i < 3; i++) fputc(toupper(ref[i]), newFile);
		//}else{
		//	fprintf(newFile, "%s", word);
		}
	}
}
