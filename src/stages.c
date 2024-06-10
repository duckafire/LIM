#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "defs.h"

char luaReserved[21][9] = {"and", "break", "do", "else", "elseif", "end", "false", "for", "function", "if", "in", "local", "nil", "not", "or", "repeat","return","then", "true", "util", "while"};

// block size: 51/BI_BLOCK (x4), 30 + 5
FILE *libTool = NULL, *libGlobal, *libLocal, *libFunc, *refeHead, *funcEnvBuf;

void startProcess(FILE **origin, FILE **newFile, char *libName, char *libNoExt){
	*newFile   = tmpfile();
	libTool    = tmpfile();
	libGlobal  = tmpfile();
	libLocal   = tmpfile();
	libFunc    = tmpfile();
	refeHead   = tmpfile();
	funcEnvBuf = tmpfile();

	// add funcions to library; add indexes to protect words
	stage_01_define(*origin, *newFile, libNoExt);
	saveState(origin, newFile, ".limfile", NULL, NULL);

	// protect strings; remove tabulations, unnecessary spaces, tabulations and line feed
	stage_02_spaces(*origin, *newFile);
	saveState(origin, newFile, ".limfile", NULL, NULL);

	// add reference to lua functions and remove "_G"
	stage_03_lualib(*origin, *newFile);
	saveState(origin, newFile, ".limfile", NULL, NULL);

	// add prefix to names and protect them together with the lua keywords
	stage_04_prefix(*origin, *newFile);
	saveState(origin, newFile, ".limfile", NULL, NULL);

	// compact words not reserved and not protected (and remote its index: '@')
	stage_05_compct(*origin, *newFile);
	saveState(origin, newFile, ".limfile", NULL, NULL);
	
	stage_06_indexr(*origin, *newFile, libNoExt);
	saveState(origin, newFile, libName, libNoExt, refeHead); // pack and references
}

static void stage_01_define(FILE *origin, FILE *newFile, char *libNoExt){
	char init[6], func[BI_BLOCK], nLib[46], cc;
	int qtt = 0;

	memset(init, '\0',  6); // local/_G
	memset(func, '\0', BI_BLOCK); // "function" and its name
	memset(nLib, '\0', 46); // store the function name without "LIB_" (No LIB prefix)

	// get local/_G
	while(fscanf(origin, "%6[^\n. ]", init) != -1){ 
		if(strcmp(init, "local") == 0 || strcmp(init, "_G") == 0){
			clearSpace(origin);
			// get function reservad word
			fscanf(origin, "%50[^\n= ]", func); // BI_BLOCK - 1

			if(strcmp(func, "function") == 0){
				clearSpace(origin);
				// get name of the function
				memset(func, '\0', BI_BLOCK);
				fscanf(origin, "%50[^(]", func); // BI_BLOCK - 1

				if(isLibFunc(func)){
					// remove "LIB_" and reorder string
					for(int i = 4; i <= strlen(func); i++) nLib[i - 4] = func[i];
					fprintf(newFile, "%s%s.%s=function%s", ID6, libNoExt, nLib, ID6);
					wordsBuffer(libTool, func);
				}else{
					fprintf(newFile, "%slocal function %s%s", ID0, func, ID0);
					wordsBuffer(libFunc, func);
				}

			}else{
				// func = variable/table name
				if(strcmp(init, "local") == 0){
					fprintf(newFile, "%s%s %s%s", ID1, init, func, ID1);
					wordsBuffer(libLocal, func);
				}else{
					// remove '.'
					for(int i = 1; i <= strlen(func); i++) func[i - 1] = func[i];

					fprintf(newFile, "%s%s%s", ID1, func, ID1);
					wordsBuffer(libGlobal, func);
				}
			}
		}else{
			fseek(origin, -strlen(init), SEEK_CUR);
		}

		qtt = 0;
		while((cc = fgetc(origin)) != '\n' && cc != EOF){
			qtt++;
			fputc(cc, newFile);
		}
		if(qtt > 0) fputc('\n', newFile);

		memset(init, '\0',  6);
		memset(func, '\0', BI_BLOCK);
		memset(nLib, '\0', 46);
	}
	// signature
	fputs("\n[#E#N#D#]", newFile); // end of file
}

static void stage_02_spaces(FILE *origin, FILE *newFile){
	// current/future character; comment block
	char cc, cf, cmt[3], lastAdded, lastLast; // #5: before last added

	// it is a commentary; it is a inverted bar
	int isCmt, isInvBar;

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
						if((cc = fgetc(origin)) == ']'){
							fseek(origin, 1, SEEK_CUR);
							break;
						}
					}
				}
			}
			// confim if it is a comment
			if(isCmt){
				continue;
			}else{
				fseek(origin, -strlen(cmt), SEEK_CUR);
			}
		}
		// string
		if(cc == '"' || cc == '\''){
			fprintf(newFile, "%s%c", ID5, cc);
			cf = cc;

			while((cc = fgetc(origin)) != EOF){
				fputc(cc, newFile);
				if(cc == cf && !isInvBar) break;
				if(cc == '\\') isInvBar = 1; else isInvBar = 0;
			}
			
			fputs(ID5, newFile);
			lastLast = lastAdded;
			lastAdded = cc;
			continue;
		}
		// spaces
		if((cc == ' ' || cc == '\t' || cc == '\n') && fCharOrNum(cf) && fCharOrNum(lastAdded)){
		   fputc(' ', newFile);
		   lastLast = lastAdded;
		   lastAdded = ' ';
		   continue;
		}
		// others
		if(cc >= 32 && cc != 127 && cc != ' '){
			fputc(cc, newFile);
			lastLast = lastAdded;
			lastAdded = cc;
		}
	}
}

static void stage_03_lualib(FILE *origin, FILE *newFile){
	// lua library; tables from 23, before only functions (basic "pack")	
	char resFunc[69][15] = {"assert", "collectgarbage", "dofile", "error", "getmetatable", "ipairs", "load", "loadfile", "next", "pairs", "pcall", "print", "rawequal", "rawget", "rawlen", "rawset", "require", "select", "setmetatable", "tonumber", "tostring", "type", "warn", "xpcall", "btn", "btnp", "circ", "circb", "clip", "cls", "elli", "ellib", "exit", "fget",  "font", "fset", "key", "keyp", "line", "map", "memcpy", "memset", "mget", "mouse", "mset", "music", "peek", "peek1", "peek2", "peek4", "pix", "pmem", "poke", "poke1", "poke2", "poke4", "rect", "rectb", "reset", "sfx", "spr",  "sync", "time", "trace", "tri", "trib", "tstamp", "ttri", "vbank"};
	
	char resTable[9][15] = {"coroutine", "debug", "io", "math", "os", "package", "utf8", "string", "table"};

	// functions to lua library tables
	char tableFunc[9][27][15] = {
		/*coroutine*/{"close", "create", "isyieldable", "resume", "running", "status", "wrap", "yield"},
		/*debug	*/{"debug", "gethook", "getinfo", "getlocal", "getmetatable", "getregistry", "getupvalue", "getuservalue", "sethook", "setlocal", "setmetatable", "setupvalue", "setuservalue", "traceback", "upvalueid", "upvaluejoin"},
		/*io	   */{"close", "flush", "input", "lines", "open", "output", "popen", "read", "stderr", "stdin", "stdout", "tmpfile", "type", "write"},
		/*math	 */{"abs", "acos", "asin", "atan", "ceil", "cos", "deg", "exp", "floor", "fmod", "huge", "log", "max", "maxinteger", "min", "mininteger", "modf", "pi", "rad", "random", "randomseed", "sin", "sqrt", "tan", "tointeger", "type", "ult"},
		/*os	   */{"clock", "date", "difftime", "execute", "exit", "getenv", "remove", "rename", "setlocale", "time", "tmpname"},
		/*package  */{"config", "cpath", "loaded", "loadlib", "path", "preload", "searchers", "searchpath"},
		/*utf8	 */{"char", "charpattern", "codepoint", "codes", "len", "offset"},
		/*string   */{"byte", "char", "dump", "find", "format", "gmatch", "gsub", "len", "lower", "match", "pack", "packsize", "rep", "reverse", "sub", "unpack", "upper"},
		/*table	*/{"concat", "insert", "move", "pack", "remove", "sort", "unpack"}
	};

	// current character; current word; current sub-word
	char cc = 'a', word[15], subw[15], refe[5];

	// booleans; indexes
	short newWord = 1, funcID, tableID, subID;

	while((cc = fgetc(origin)) != EOF){
		// indexed with "@n"
		if(protectedWords(origin, newFile, cc, 1) > -1) continue;

		memset(word, '\0', 15);
		memset(subw, '\0', 15);
		memset(refe, '\0',  5);

		funcID = tableID = subID = -1;

		// metamethods
		if(cc == '_'){
			cc = fgetc(origin);
			
			// remove "_G."
			if(cc == 'G'){
				fgetc(origin);
				continue;
			}

			// protect metamethod
			if(cc == '_'){
				fscanf(origin, "%15[a-z]", word);
				fprintf(newFile, "%s__%s%s", ID2, word, ID2);
				continue;
			}
			fseek(origin, -2, SEEK_CUR);
			cc = fgetc(origin);
		}

		// cc = (a-z)
		if(newWord && cc >= 97 && cc <= 122){
			newWord = 0;

			// compense "cc"
			fseek(origin, -1, SEEK_CUR);
			
			// get word finded
			fscanf(origin, "%15[a-zA-Z8_]", word);

			for(int i = 0; i < 69; i++){
				if(strcmp(word, resFunc[i]) == 0){
					funcID = i;
					break;
				}
			}

			// search reserved word
			for(int i = 0; i < 9; i++){
				// word was finded
				if(funcID != -1) break;

				if(strcmp(word, resTable[i]) == 0){
					tableID = i; // word index

					// get '.'; if it will not finded, it is not a valid lua table-functions
					if((cc = fgetc(origin)) != '.'){
						//fputc('#', newFile);
						break;
					}

					// get sub word finded
					fscanf(origin, "%15[a-zA-Z8_⁽]", subw);

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
		if(!firstChar(cc) || isNum(cc) || cc == '_') newWord = 1;
		
		// only function
		if(funcID != -1){
			sprintf(refe, "%c%d\0", toupper(word[0]), funcID);

			fprintf(newFile, "%s%s%s", ID2, refe, ID2);
			
			refeBuffer(refeHead, word, NULL, refe);
			continue;
		}

		// table with function
		if(tableID != -1 && subID != -1){
			sprintf(refe, "%c%c%d\0", toupper(word[0]), toupper(subw[0]), subID);

			fprintf(newFile, "%s%s%s", ID3, refe, ID3);

			refeBuffer(refeHead, subw, word, refe);
			continue;
		}

		// word finded but it it not a reserved word
		if(word[0] != '\0' || tableID != -1){
			fprintf(newFile, "%s", word);

			if(tableID != -1){
				fseek(origin, -1, SEEK_CUR);
				for(int i = 0; i <= 1; i++) fputc(fgetc(origin), newFile);
			}
			continue;
		}

		// it is not a valid character
		fputc(cc, newFile);
	}
}

static void stage_04_prefix(FILE *origin, FILE *newFile){
	//char luaReserved[21][9] = {"and", "break", "do", "else", "elseif", "end", "false", "for", "function", "if", "in", "local", "nil", "not", "or", "repeat","return","then", "true", "util", "while"};
	
	short block, protectionID = 0, insideEnv = 0, isReserved = 0, isTable = 0;
	char cc, last = ' ', word[BI_BLOCK], readWord[BI_BLOCK];
	char startBlock[4][9] = {"do", "function", "if", "end"};
	FILE *buffers[5] = {libTool, libGlobal, libLocal, libFunc, funcEnvBuf};

	while((cc = fgetc(origin)) != EOF){
		memset(word, '\0', BI_BLOCK);

		// TRUE end of file
		if(cc == '['){
			fscanf(origin, "%8[#END]", word);
			if(strcmp(word, "#E#N#D#") == 0 && fgetc(origin) == ']'){
				fprintf(newFile, "[%s]", word);
				break;
				}
			fseek(origin, -(strlen(word) + 1), SEEK_CUR);
		}

		// indexed with "@n"
		if((protectionID =  protectedWords(origin, newFile, cc, 1) - 48) > -1){
			if(protectionID == 0 || protectionID == 6){
				fclose(funcEnvBuf);
				funcEnvBuf = tmpfile();
				insideEnv = !insideEnv;
			}
			last = cc;
			continue;
		}

		// get if it is inside a table
		if(cc == '{') isTable++;
		if(cc == '}') isTable--;

		if(!firstChar(cc) || fCharOrNum(last) || last == '.'){
			fputc(cc, newFile);
			last = cc;
			continue;
		}

		fseek(origin, -1, SEEK_CUR);
		fscanf(origin, "%50[a-zA-Z0-9_]", word); // BI_BLOCK - 1
		last = word[0];

		isReserved = 0;
		for(int i = 0; i < 21; i++){
			if(strcmp(word, luaReserved[i]) == 0){
				isReserved = 1;
				break;
			}
		}

		// it is a table element
		if(isTable > 0){
			if(fgetc(origin) == '=') isReserved = 1; // *it is a table element
			fseek(origin, -1, SEEK_CUR);
		}

		if(!isReserved){
			// check if is a reserved word
			isReserved = -1;

			for(int i = 0; i < 5; i++){
				fseek(buffers[i], 0, SEEK_SET);
				for(block = 0; fread(readWord, sizeof(readWord), 1, buffers[i]); block++){
					if(strcmp(word, readWord) == 0){
						if(i < 4) fprintf(newFile, "%s%s%s", ID7, word, ID7); // protection
						isReserved = i;
						break;
					}
				}
				if(isReserved != -1) break;
			}
			
			// it was defined and it is a protected name
			if(isReserved != -1 && isReserved != 4) continue;

			fseek(funcEnvBuf, 0, SEEK_END);

			// add name to buffer
			if(isReserved == -1){
				memset(readWord, '\0', sizeof(readWord));
				strcpy(readWord, word);
				fwrite(readWord, sizeof(readWord), 1, funcEnvBuf);
			}

			// write it in the file
			if(block < 26){ // a-z -> a0-z0 -> an...-zn...
				fprintf(newFile, "%c%s", 97 + block, word);
			}else{
				fprintf(newFile, "%c%d%s", 97 + block, block / 26, word);
			}
			continue;
		}
		fprintf(newFile, "%s%s%s", ID7, word, ID7);
	}
}

static void stage_05_compct(FILE *origin, FILE *newFile){
	char cc, word[BI_BLOCK];

	while((cc = fgetc(origin)) != EOF){
		// indexed with "@n"; content between '{' '}'
		if(protectedWords(origin, newFile, cc, 0) > -1) continue;
		
		memset(word, '\0', BI_BLOCK);

		if(cc == '.'){
			fputc('.', newFile);
			while(fCharOrNum((cc = fgetc(origin))) && cc != EOF) fputc(cc, newFile);
		}

		if(!firstChar(cc)){
			fputc(cc, newFile);
			continue;
		}

		fseek(origin, -1, SEEK_CUR);
		fscanf(origin, "%50[a-zA-Z0-9_]", word); // BI_BLOCK - 1

		// write prefix (ex: a0, j, g98)
		fputc(word[0], newFile); // letter
		for(int i = 1; i < strlen(word); i++){
			if(!isNum(word[i])) break;
			fputc(word[i], newFile); // number code
		}
	}
}

static void stage_06_indexr(FILE *origin, FILE *newFile, char *libNoExt){
	char cc, funcName[BI_BLOCK];
	
	while((cc = fgetc(origin)) != EOF){
		if(cc == 'L'){
			if(fgetc(origin) == 'I' && fgetc(origin) == 'B' && fgetc(origin) == '_'){
				fscanf(origin, "%50[^(]", funcName); // BI_BLOCK - 1
				fprintf(newFile, "%s.%s", libNoExt, funcName);
				continue;
			}
			fseek(origin, -3, SEEK_CUR);
		}
		fputc(cc, newFile);
	}
}

void cleanupWrite(void){
	// by security
	if(libTool != NULL){
		fclose(libTool);
		fclose(libGlobal);
		fclose(libLocal);
		fclose(libFunc);
		fclose(refeHead);
		fclose(funcEnvBuf);
	}
}

// nomes de variáveis e tabelas globais não respeitado
// remover prefix para funções
