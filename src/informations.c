#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include "heads.h"

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

void info_help(char *arg){
	if(arg == NULL || t_strcmp2(arg, F_HELP))
		message(0,
			"lim {-h | --help} [arg]",
			" ",
			"Print informations about a specific argument, that can be a flag or one of the"
			"keywords in below:",
			"* (h) header   -> how work and how to use \"header.lim\".",
			"* (l) list     -> list all flags.",
			"* (li)license  -> program source code license.",
			"* (ls)list-syn -> list all flag with their recommended flags",
			"* (r) rules    -> semantic rules to source file.",
			"* (s) synopsis -> recommended synopsis for all arguments to Lim.",
		NULL);
	else if(t_strcmp2(arg, HELP_ARG_LIST))
		message(0,
			"-v  --version",
			"-h  --help",
			"-V  --verbose",
			"-n  --name",
			"-r  --replace",
			"-nh --no-header",
			"-us --until-stage",
		NULL);
	else if(t_strcmp2(arg, HELP_ARG_LICE))
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
	else if(t_strcmp2(arg, HELP_ARG_RULE))
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
	else if(t_strcmp2(arg, HELP_ARG_HEAD))
		message(0,
			"The \"header.lim\" file is search always that the Lim run, unless the flag \"(-nh)",
			"--no-header\" is used. When it is found, the Lim read, separate and store its",
			"content in a buffer, for future use.",
			" ",
			"The header content is divided in three partitions, they are separated by \"\\n@\".",
			"All partitions are listed below:",
			" ",
			"* Top -> its content will not be compacted, it will be pritted in top of output",
			"file, above the library code.",
			" ",
			"* Scope -> code, extern to the source file, that will be placed below references",
			"functions scope. It content will not be compacted, because of this, it is",
			"recommended that its content already has be compacted.",
			" ",
			"* List -> functions, extern of Lua default collection, that may be present in",
			"source file.",
			" ",
			"If the \"header.lim\", or some of its partition, is not found, the Lim will not be",
			"stop and it will be continue the compaction process. This can be monitored with the",
			"use of the flag \"(-V) --verbose\".",
			" ",
			"Structure synopsis:",
			" ",
			"-- Top of header file",
			"@",
			"local function add(a,b)return a+b end",
			"@",
			"rectangle",
			"rectangleBoard",
		NULL);
	else if(t_strcmp2(arg, HELP_ARG_SYNO))
		message(0,
			"lim [-v] [-h [help-arg]] [-V] [-r] <src-name> [-nh] [-us <1 | 2 | 3 | 4>] [-n <dest-name>]",
		NULL);
	else if(t_strcmp2(arg, HELP_ARG_LISY))
		message(0,
			"-v  --version     -> lim {-v | --version}",
			"-h  --help        -> lim {-h | --help} [arg]",
			"-V  --verbose     -> lim {-V | --verbose} <source-name>",
			"-n  --name        -> lim <source-name> {-n | --name} <destine-name>",
			"-r  --replace     -> lim {-r | --replace} <source-name>",
			"-nh --no-header   -> lim <source> {-nh | --no-header} <source-name>",
			"-us --until-stage -> lim <source-name> {-us | --until-stage} <1 | 2 | 3 | 4>",
		NULL);
	else if(t_strcmp2(arg, F_VERSION))
		message(0,
			"lim {-v | --version}",
			" ",
			"Print the program version",
		NULL);
	else if(t_strcmp2(arg, F_HELP))
		message(0,
			"lim {-h | --help} [arg]",
			" ",
			"Print informations about main args or about a specific arg.",
		NULL);
	else if(t_strcmp2(arg, F_VERBOSE))
		message(0,
			"lim {-V | --verbose} <source-name>",
			" ",
			"Print informations about the compaction, during this process.",
		NULL);
	else if(t_strcmp2(arg, F_NAME))
		message(0,
			"lim <source-name> {-n | --name} <destine-name>",
			" ",
			"Specify that the next argument is the output file name.",
		NULL);
	else if(t_strcmp2(arg, F_REPLACE))
		message(0,
			"lim {-r | --replace} <source-name>",
			" ",
			"Specify that, if already exist a file with the same name of the",
			"output file, it must be replaced.",
		NULL);
	else if(t_strcmp2(arg, F_NO_HEAD))
		message(0,
			"lim <source> {-nh | --no-header} <source-name>",
			" ",
			"Specific that the file \"header.lim\" must be ignored.",
		NULL);
	else if(t_strcmp2(arg, F_UNTIL_S))
		message(0,
			"lim <source-name> {-us | --until-stage} <1 | 2 | 3 | 4>",
			" ",
			"Specific that the compaction process must run until a specific stage.",
		NULL);
	else{
		er_invalidArgToHelp(arg);
	}

	exit(0);
}

void info_verbose(short mode, ...){
	if(!flags.verbose) return;

	va_list text;
	va_start(text, mode);

	if(mode == VM_NORMAL)
		printf("[LIM] %s\n", va_arg(text, char*));

	else if(mode == VM_STAGE){
		int d;
		char *s;
		d = va_arg(text, int);
		s = va_arg(text, char*);

		printf("\n[LIM] STAGE %d started: %s\n", d, s);

	}else if(mode == VM_PROCESS)
		printf("[LIM] Process of read and %s started.\n", va_arg(text, char*));

	else if(mode == VM_FREE)
		printf("[LIM] Freeing allocated memory(ies).\n");

	else if(mode == VM_BREAK_STAGE)
		printf("\n[LIM] Program break; stopped in Stage %d.\n\t", va_arg(text, int));

	else if(mode == VM_HEADER)
		printf("[LIM] Loading \"header.lim\": ...\n\t%s\n", va_arg(text, char*));

	else{
		char *cur;

		if(mode == VM_BREAK_FREE)
			printf("> Finishing buffers:");

		else if(mode == VM_BUFFER_INIT)
			printf("[LIM] Starting buffer:");

		else
			printf("[LIM] Finishing buffer:");

		while((cur = va_arg(text, char*)) != NULL)
			printf(" \"%s\";", cur);

		puts(" ...");

		if(mode == VM_BREAK_FREE)
			putchar('\n');
	}

	va_end(text);
}
