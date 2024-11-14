#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include "../heads.h"

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
		fprintf(stdout, "[LIM] %s\n", msg);
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
	if(tools_strcmp2(flag, F_VERSION))
		message(0,
			"lim {-v | --version}",
			" ",
			"Print the program version",
		NULL);
	else if(tools_strcmp2(flag, F_H_LIST ))
		message(0,
			"lim {-hl | --help-list}",
			" ",
			"Print all flags, in list format, without description",
		NULL);
	else if(tools_strcmp2(flag, F_HELP   ))
		message(0,
			"lim {-h | --help} [flag]",
			" ",
			"Print informations about main flags or about a specific flag.",
		NULL);
	else if(tools_strcmp2(flag, F_VERBOSE))
		message(0,
			"lim {-V | --verbose} <origin>",
			" ",
			"Print informations about the compaction, during this process.",
		NULL);
	else if(tools_strcmp2(flag, F_NAME   ))
		message(0,
			"lim <origin> {-n | --name} <destine-name>",
			" ",
			"Specify that the next argument is the output file name.",
		NULL);
	else if(tools_strcmp2(flag, F_REPLACE))
		message(0,
			"lim {-r | --replace} <origin>",
			" ",
			"Specify that, if already exist a file with the same name of the",
			"output file, it must be replaced.",
		NULL);
	else if(tools_strcmp2(flag, F_LICENSE))
		message(0,
			"lim {-l | --license}",
			" ",
			"Print the program license.",
		NULL);
	else if(tools_strcmp2(flag, F_RULES))
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

	if(verboseMsg(0, "COMPACTION STARTED!")) return;
	if(verboseMsg(1, "STAGE 0: check specified files.")) return;
	if(verboseMsg(2, "Checking origin file...")) return;
	if(g_replace){
		if(verboseMsg(3, "REPLACE: ON => checking if destine file already exist...")) return;
	}else{
		if(verboseMsg(3, "REPLACE: OFF =x")) return;
	}
	if(verboseMsg(4, "STAGE 1: extract content from origin.")) return;
	if(verboseMsg(5, "Starting buffer: \"collect\"; \"ident\".")) return;
	if(verboseMsg(6, "Read and extract process started.")) return;
	if(verboseMsg(7, "Read and extract process finished.")) return;
	if(verboseMsg(8, "Ending buffer: \"ident\".")) return;
	if(verboseMsg(9, "STAGE 2: separate extracted content.")) return;
	if(verboseMsg(10, "Starting buffer: \"ident\"; \"global\".")) return;
	if(verboseMsg(11, "Getting extracted content...")) return;
	if(verboseMsg(12, "Ending buffer: \"collect\".")) return;
	if(verboseMsg(13, "Read and separate process started.")) return;
	if(verboseMsg(14, "Read and separate process finished.")) return;
	if(verboseMsg(15, "Ending buffer: \"ident\".")) return;
	if(verboseMsg(16, "STAGE 3: building global scope to FUNCTIONs REFERENCES.")) return;
	if(verboseMsg(17, "Starting buffer: \"ident\", \"refe\".")) return;
	if(verboseMsg(18, "Read and build process started.")) return;
	if(verboseMsg(19, "Read and build process finished.")) return;
	if(verboseMsg(20, "- WORK IN PROGRESS] Ending buffer: \"ident\", \"refe\".")) return;
	if(verboseMsg(21, "- WORK IN PROGRESS] Alpha process finished!")) return;
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
		"Compaction rules (work in progress):",
		" ",
		"1 - Global variables and tables must ALWAYS to be prefixed by \"_G.\".",
		"2 - Global variables, tables and functions will be added to the library table.",
		"3 - Variables, tables and functions declared inside aligned functions must not have",
		"    the same name.",
		"4 - \"L\" is a reservated identifier to appoint the library table, because of this,",
		"    it must not be used to other purposes.",
		"5 - Variables and tables, global and local, declared in root environment will be",
		"    hoisted to the top of this environment, like what happen with variables declared",
		"    with \"var\" in JavaScript.",
		"6 - Global variables, tables and function, and table keys, will not be their",
		"    identifiers compacted.",
		"7 - Functions declared inside of tables environment will not be their content",
		"    currectly compacted.",
		"    > Try: <table>.<ident>=function()end",
		"    > Instead <table>={<ident>=function()end}",
		"8 - The use of \"[[]]\", to declare string, is not supported (use \"\\n\" to apply",
		"    line feed).",
	NULL);

	exit(0);
}
