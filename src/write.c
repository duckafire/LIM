#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "defs.h"

void printInFile(FILE *origin, FILE *newFile, char *libName){
	// get library functions, variables and tables; remove unnecessary line feed
	// stage_01_define(origin, newFile, libName);
	
	// search lua libraries and replce them by refences; remove unnecessary tabulations
	stage_02_lualib(origin, newFile);
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
    char resMain[33][15] = { "assert", "collectgarbage", "dofile", "error", "getmetatable", "ipairs", "load", "loadfile", "next", "pairs", "pcall", "print", "rawequal", "rawget", "rawlen", "rawset", "require", "select", "setmetatable", "tonumber", "tostring", "type", "warn", "xpcall", "coroutine", "debug", "math", "os", "package", "io", "utf8", "string", "table"};
	
	// functions to lua library tables
	char resSub[9][27][15] = {
		/*courotine*/{"close", "create", "isyieldable", "resume", "running", "status", "wrap", "yield"},
		/*debug    */{"debug", "gethook", "getinfo", "getlocal", "getmetatable", "getregistry", "getupvalue", "getuservalue", "sethook", "setlocal", "setmetatable", "setupvalue", "setuservalue", "traceback", "upvalueid", "upvaluejoin"},
		/*math     */{"abs", "acos", "asin", "atan", "ceil", "cos", "deg", "exp", "floor", "fmod", "huge", "log", "max", "maxinteger", "min", "mininteger", "modf", "pi", "rad", "random", "randomseed", "sin", "sqrt", "tan", "tointeger", "type", "ult"},
		/*os       */{"clock", "date", "difftime", "execute", "exit", "getenv", "remove", "rename", "setlocale", "time", "tmpname"},
		/*package  */{"config", "cpath", "loaded", "loadlib", "path", "preload", "searchers", "searchpath"},
		/*io       */{"close", "flush", "input", "lines", "open", "output", "popen", "read", "tmpfile", "type", "write"},
		/*utf8     */{"char", "charpattern", "codepoint", "codes", "len", "offset"},
		/*string   */{"byte", "char", "dump", "find", "format", "gmatch", "gsub", "len", "lower", "match", "pack", "packsize", "rep", "reverse", "sub", "unpack", "upper"},
		/*table    */{"concat", "insert", "move", "pack", "remove", "sort", "unpack"}
	};

	// current character; current word; current sub-word
	char cc = 'a', word[15], subw[15];

	// one new word was finded; index to "main" word; index to "sub" word
	int newWord = 1, mainID = 0, subID = 0;

	while((cc = fgetc(origin)) != EOF){
		memset(word, '\0', 15);
		memset(subw, '\0', 15);

		mainID = subID = -1;

		// cc = (a-z)
		if(newWord && cc >= 97 && cc <= 122){
            newWord = 0;

			// compense "cc"
			fseek(origin, -1, SEEK_CUR);
			
			// get word finded
			fscanf(origin, "%15[^\n.(]", word);

			// serach reserved word
			for(int i = 0; i < 33; i++){
                fprintf(stdout, "[%s - %s] \n", word, resMain[i]);
				if(strcmp(word, resMain[i]) == 0){
					mainID = i; // word index
						
					 // check if it is a "table library"
					 if(mainID >= 24){
						// jump '.'
						fseek(origin, 1, SEEK_CUR);

						// get sub word finded
						fscanf(origin, "%15[^\n(]", subw);

						// search reserved sub word
                        for(int j = 0; j < 27; j++){
							// void strings
                            if(subw[0] < 32 || resSub[i - 22][j][0] < 32) break;
						    
                            // compare
                            if(strcmp(subw, resSub[i - 23][j]) == 0){
                                subID = j; // sub index
								break;
							}
						}
					}
				break; // 'i'
				}
			}
		}
	
		// finded end of the last word
		if(cc == ' ' || cc == '\n' || cc == '\t') newWord = 1;
		
		// without function
		if(mainID >= 0 && mainID < 24){
            fprintf(newFile, "+%c%d", toupper(word[0]), mainID);
			continue;
		}

		// with function
		if(mainID >= 24 && subID > -1){
			fprintf(newFile, "~%c%c%d", toupper(word[0]), toupper(subw[0]), subID);
			continue;
		}

		// word finded but it it not a reserved word
		if(word[0] != '\0'){
			fprintf(newFile, "^%s", word);
				
			if(mainID >= 24 && subID == 0) fputc('.', newFile); // '.' jumped
		    continue;
        }

		// it is not a valid character
		fputc(cc, newFile);
	}
}
