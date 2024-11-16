#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include "../heads.h"

static void message(char n, ...){
	va_list parag;
	va_start(parag, n);

	char *cur;

	while((cur = va_arg(parag, char*)) != NULL)
		printf("%s\n", cur);

	putchar('\n');
	va_end(parag);
	exit(0);
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

void info_verbose(short mode, ...){
	if(!g_verbose) return;

	char *cur;        // #3
	char content[16]; // #2 & #3
	va_list text;     // #all

	va_start(text, mode);

	// #1: [LIM] ~
	if(mode == VM_NORMAL || mode == VM_TITLE){
		char lf = (mode == VM_TITLE) ? '\n' : '\0';

		printf("%c[LIM] %s\n", lf, va_arg(text, char*));
		va_end(text);
		return;
	}
	
	// #2: [LIM] Read and ~ process started/finished.
	if(mode == VM_START_PRO || mode == VM_END_PRO){
		if(mode == VM_START_PRO)
			strcpy(content, "started.\0");
		else
			strcpy(content, "finished.\0");

		printf("[LIM] Read and %s process %s", va_arg(text, char*), content);

		putchar('\n');
		va_end(text);
		return;
	}

	// #3: [LIM] Starting/Ending buffer: "~0"; "~n";
	if(mode == VM_START_BUF)
		strcpy(content, "Starting\0");
	else
		strcpy(content, "Ending\0");

	printf("[LIM] %s buffer:", content);

	while((cur = va_arg(text, char*)) != NULL)
		printf(" \"%s\";", cur);

	putchar('\n');
	va_end(text);
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
