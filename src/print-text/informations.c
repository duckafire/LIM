#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include "sub-heads.h"

static short verboseID = -1;

static void message(char n, ...){
	va_list parag;
	va_start(parag, n);

	char *cur;

	while((cur = va_arg(parag, char*)) != NULL)
		fprintf(stdout, "%s\n", cur);

	fputc('\n', stdout);
	va_end(parag);
	exit(0);
}

static bool verboseMsg(short value, char *msg){
	if(verboseID == value){
		fprintf(stdout, "[LIM] %s.\n", msg);
		return true;
	}
	return false;
}

void info_welcome(void){
	message(0,
		"[LIM] - Lua Library Compactor",
		" ",
		"Try: \"lim --help\"",
	NULL);
	exit(0);
}

void info_version(void){
	message(0, LIM_VERSION, NULL);
	exit(0);
}

void info_helpList(void){
	message(0,
		"[LIM] Commands list:",
		" ",
		"-v  --version",
		"-hl --help-list",
		"-h  --help",
		"-V  --verbose",
		"-n  --name",
		"-r  --replace",
		"-l  --license",
		"-R  --rules",
	NULL);
	exit(0);
}

void info_help(char *flag){
	if(strcmp2(flag, F_VERSION))
		message(0,
			"lim {-v | --version}",
			" ",
			"Print the program version",
		NULL);
	else if(strcmp2(flag, F_H_LIST ))
		message(0,
			"lim {-hl | --help-list}",
			" ",
			"Print all flags, in list format, without description",
		NULL);
	else if(strcmp2(flag, F_HELP   ))
		message(0,
			"lim {-h | --help} [flag]",
			" ",
			"Print informations about main flags or about a specific flag.",
		NULL);
	else if(strcmp2(flag, F_VERBOSE))
		message(0,
			"lim {-V | --verbose} <origin>",
			" ",
			"Print informations about the compaction, during this process.",
		NULL);
	else if(strcmp2(flag, F_NAME   ))
		message(0,
			"lim <origin> {-n | --name} <destine-name>",
			" ",
			"Specify that the next argument is the output file name.",
		NULL);
	else if(strcmp2(flag, F_REPLACE))
		message(0,
			"lim {-r | --replace} <origin>",
			" ",
			"Specify that, if already exist a file with the same name of the",
			"output file, it must be replaced.",
		NULL);
	else if(strcmp2(flag, F_LICENSE))
		message(0,
			"lim {-l | --license}",
			" ",
			"Print the program license.",
		NULL);
	else if(strcmp2(flag, F_RULES))
		message(0,
			"lim {-R | --rules}",
			" ",
			"Print compaction rules.",
		NULL);
	else
		message(0,
			"[LIM] main flags:",
			"lim {-h | --help} [flag]             => Print informations about main flags or",
			"                                        about a specific flag.",
			"lim <org> {-n | --name} <dst-name>   => Specify that the next argument is the",
			"                                        the output file name.",
			"lim {-r | --replace} <org>           => Specify that, if already exist a file",
			"                                        with the same name of the output file,",
			"                                        it must be replaced.",
			"lim {-l | --license}                 => Print the program license.",
			"lim {-R | --rules}                   => Print compaction rules.",
		NULL);

	exit(0);
}

void info_verbose(void){
	if(!g_verbose) return;

	verboseID++;

	if(verboseMsg(0, "[LIM] Compaction started")) return;
	if(verboseMsg(1, "[LIM] Extracting content from origin")) return;
	//if(verboseMsg(2, "[LIM] Separating content in buffers, base in them type")) return;
	//if(verboseMsg(3, "[LIM] Creating references functions from Lua, Tic80 and 'head'")) return;
	//if(verboseMsg(4, "[LIM] Compacting 'private' indentifiers")) return;
	//if(verboseMsg(5, "[LIM] Compacting environments from blocks of code")) return;
	//if(verboseMsg(6, "[LIM] Merging content buffers")) return;
	//if(verboseMsg(7, "[LIM] Packing code")) return;
	//if(verboseMsg(8, "[LIM] Process finished!")) return;
	
	// temp
	if(verboseMsg(2, "[LIM-alpha-debug] Process finished (alpha)")) return;
	if(verboseMsg(3, "[LIM-alpha-debug] See the output in file: \"output.lim\"")) return;
}

void info_license(void){
	message(0,
		"MIT License",
		" ",
		"Copyright (c) 2024 DuckAfire <duckafire.github.io/nest>",
		" ",
		"Permission is hereby granted, free of charge, to any person obtaining a copy",
		"of this software and associated documentation files (the \"Software\"), to deal",
		"in the Software without restriction, including without limitation the rights",
		"to use, copy, modify, merge, publish, distribute, sublicense, and/or sell",
		"copies of the Software, and to permit persons to whom the Software is",
		"furnished to do so, subject to the following conditions:",
		" ",
		"The above copyright notice and this permission notice shall be included in all",
		"copies or substantial portions of the Software.",
		" ",
		"THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR",
		"IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,",
		"FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE",
		"AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER",
		"LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,",
		"OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE",
		"SOFTWARE.",
	NULL);
	exit(0);
}

void info_rules(void){
	message(0,
		"Compaction rules:",
		" ",
		"1 - Only global functions will be annexed to the library.",
		"2 - Global variables, tables and functions will not have them identifiers compacted.",
		"3 - The identifiers annexed to the tables will not be compacted.",
		"4 - Global variables and tables need to be declared with the prefix '_G'.",
		"5 - The use of '[[ ]]', to declare strings, is not supported (use '\\n').",
	NULL);

	exit(0);
}
