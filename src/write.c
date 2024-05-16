#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "defs.h"

void printInFile(FILE *origin, FILE *newFile, FILE **fileName, char *libName){
	// get library functions, variables and tables; remove unnecessary line feed
	stage_01_define(origin, newFile, libName);
	
    rewind(origin);
    rewind(newFile);

	// search lua libraries and replce them by refences; remove unnecessary some tabulations
	stage_02_lualib(origin, newFile);

    //rewind(origin);
    //rewind(newFile);
    
    // remove the last tabuleations, line feed, comments and unnecessary spaces
    //stage_03_spaces(origin, newFile, fileName, libName);
    
    //rewind(origin);
    //rewind(newFile);
    
    // ???
    //stage_04_cmpact(origin, newFile);
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

				fprintf(newFile, "@%s.%s=function@", lib, name);
			}else{
				fseek(origin, -strlen(func), SEEK_CUR);
				fprintf(newFile, "@%s @", init);
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
    char resFunc[69][15] = {"assert", "collectgarbage", "dofile", "error", "getmetatable", "ipairs", "load", "loadfile", "next", "pairs", "pcall", "print", "rawequal", "rawget", "rawlen", "rawset", "require", "select", "setmetatable", "tonumber", "tostring", "type", "warn", "xpcall", "btn", "btnp", "circ", "circb", "clip", "cls", "elli", "ellib", "exit", "fget",  "font", "fset", "key", "keyp", "line", "map", "memcpy", "memset", "mget", "mouse", "mset", "music", "peek", "peek1", "peek2", "peek4", "pix", "pmem", "poke", "poke1", "poke2", "poke4", "rect", "rectb", "reset", "sfx", "spr",  "sync", "time", "trace", "tri", "trib", "tstamp", "ttri", "vbank"};
	
    char resTable[9][15] = {"coroutine", "debug", "io", "math", "os", "package", "utf8", "string", "table"};

	// functions to lua library tables
	char tableFunc[9][27][15] = {
        /*coroutine*/{"close", "create", "isyieldable", "resume", "running", "status", "wrap", "yield"},
		/*debug    */{"debug", "gethook", "getinfo", "getlocal", "getmetatable", "getregistry", "getupvalue", "getuservalue", "sethook", "setlocal", "setmetatable", "setupvalue", "setuservalue", "traceback", "upvalueid", "upvaluejoin"},
		/*io       */{"close", "flush", "input", "lines", "open", "output", "popen", "read", "stderr", "stdin", "stdout", "tmpfile", "type", "write"},
		/*math     */{"abs", "acos", "asin", "atan", "ceil", "cos", "deg", "exp", "floor", "fmod", "huge", "log", "max", "maxinteger", "min", "mininteger", "modf", "pi", "rad", "random", "randomseed", "sin", "sqrt", "tan", "tointeger", "type", "ult"},
		/*os       */{"clock", "date", "difftime", "execute", "exit", "getenv", "remove", "rename", "setlocale", "time", "tmpname"},
		/*package  */{"config", "cpath", "loaded", "loadlib", "path", "preload", "searchers", "searchpath"},
		/*utf8     */{"char", "charpattern", "codepoint", "codes", "len", "offset"},
		/*string   */{"byte", "char", "dump", "find", "format", "gmatch", "gsub", "len", "lower", "match", "pack", "packsize", "rep", "reverse", "sub", "unpack", "upper"},
		/*table    */{"concat", "insert", "move", "pack", "remove", "sort", "unpack"}
	};

	// current character; current word; current sub-word
	char cc = 'a', word[15], subw[15];

    // booleans; indexes
	int newWord = 1, funcID, tableID, subID;

	while((cc = fgetc(origin)) != EOF){
		memset(word, '\0', 15);
		memset(subw, '\0', 15);

		funcID = tableID = subID = -1;

        if(cc == '_'){
            if((cc = fgetc(origin)) == '_'){
                fscanf(origin, "%15[a-z]", word);
                fprintf(newFile, "@__%s@", word);
                continue;
            }
            fseek(origin, -1, SEEK_CUR);
        }

		// cc = (a-z)
		if(newWord && cc >= 97 && cc <= 122){
            newWord = 0;

			// compense "cc"
			fseek(origin, -1, SEEK_CUR);
			
			// get word finded
			fscanf(origin, "%15[^\n.( ]", word);

            for(int i = 0; i < 69; i++){
                if(strcmp(word, resFunc[i]) == 0){
                    funcID = i;
                    break;
                }
            }

            // serach reserved word
			for(int i = 0; i < 9; i++){
				// word was finded
                if(funcID != -1) break;

                if(strcmp(word, resTable[i]) == 0){
					tableID = i; // word index

					// jump '.'
					fseek(origin, 1, SEEK_CUR);

					// get sub word finded
					fscanf(origin, "%15[^\n(]", subw);

					// search reserved sub word
                    for(int j = 0; j < 27; j++){
						// void strings
                        if(subw[0] < 32 || tableFunc[i][j][0] < 32) break;
						    
                        // compare
                        if(strcmp(subw, tableFunc[i][j]) == 0){
                            subID = j; // sub index
                            break;
						}
					}
				break; // 'i' 2
				}
			}
		}
		// finded end of the last word
		if(cc == ' ' || cc == '\n' || cc == '\t') newWord = 1;
		
		// only function
		if(funcID != -1){
            fprintf(newFile, "@%c%d@", toupper(word[0]), funcID);
			continue;
		}

		// table with function
		if(tableID != -1 && subID != -1){
			fprintf(newFile, "@%c%c%d@", toupper(word[0]), toupper(subw[0]), subID);
			continue;
		}

		// word finded but it it not a reserved word
		if(word[0] != '\0' || tableID != -1){
			fprintf(newFile, "@%s@", word);
				
			if(subID == -1) fputc('.', newFile); // '.' jumped
		    continue;
        }

		// it is not a valid character
		fputc(cc, newFile);
	}
}

static void stage_03_spaces(FILE *origin, FILE *newFile, FILE **newAdress, char *fileName){
    // current/future character; comment
    char cc, cf, cmt[3];

    // it is a commentary; it is a inverted bar
    int isCmt, isInvBar;

    // to read "newFile" (library will that compacted)
    FILE *readNew;

    // remove unnecessary spaces, respecting string; remove comments
    while((cc = fgetc(origin)) != EOF){
        if(cf != EOF){
            cf = fgetc(origin);
            fseek(origin, -1, SEEK_CUR);
        }

        // comments
        if(cc == '-'){
            // line
            if(fgetc(origin) == '-' && fgetc(origin) != '['){
                while(fgetc(origin) != '\n');
                continue;
            }
            // block
            isCmt = 0;
            fseek(origin, -1, SEEK_CUR);
            memset(cmt, '\0', 4);
            fscanf(origin, "%2s", cmt);

            if(strcmp(cmt, "[[") == 0){
                isCmt = 1;
                while((cc = fgetc(origin)) != EOF){
                    if(cc = ']'){
                        if((cc = fgetc(origin)) == ']') break;
                    }
                }
            }

            if(isCmt){
                continue;
            }else{
                fseek(origin, -strlen(cmt), SEEK_CUR);
            }
        }
        // string
		if(cc == '"' || cc == '\''){
            fputc(cc, newFile);
			cf = cc;

			while((cc = fgetc(origin)) != EOF){
				fputc(cc, newFile);
				if(cc == cf && !isInvBar) break;
				if(cc == '\\') isInvBar = 1; else isInvBar = 0;
            }
			
			continue;
		}
        // spaces
        if((cc == ' ' || cc == '\t' || cc == '\n') && firstChar(cf)){
            ///*
            fclose(newFile);
            *newAdress = fopen(fileName, "a");
            newFile = *newAdress;

            readNew = fopen(fileName, "r");
            fseek(readNew, -1, SEEK_END);
            if(firstChar(fgetc(readNew))) fputc(' ', newFile);
            fclose(readNew);
            //*/
            //fputc(' ', newFile);
            continue;
        }
        // others
        if(cc != ' ' && cc != '\t' && cc != '\n'){
            fputc(cc, newFile);
        }
    }
}

static void stage_04_cmpact(FILE *origin, FILE *newFile){
    // "BDR", "BOOT", "MENU", "SCN", "TIC"

    // add "@" to init of ALL reserved words, in ALL stages

    // ADD
    // // references
    // // pack
    // // table
}
