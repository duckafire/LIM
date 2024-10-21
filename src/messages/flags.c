#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#include "../head.h"
#include "head.h"

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

void welcome(void){
	message(0,
		"[LIM] - Lua Library Compactor",
		" ",
		"Try: \"lim --help\"",
	NULL);
	exit(0);
}

void version(void){
	message(0, LIM_VERSION, NULL);
	exit(0);
}

void helpList(void){
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
	NULL);
	exit(0);
}

void help(char *flag){
	if(strcmp2(flag, F_VERSION))
		message(0,
			"-v --version",
			" ",
			"Comming soon...",
		NULL);
	else if(strcmp2(flag, F_H_LIST ))
		message(0,
			"-hl --help-list",
			" ",
			"Comming soon...",
		NULL);
	else if(strcmp2(flag, F_HELP   ))
		message(0,
			"-h --help",
			" ",
			"Comming soon...",
		NULL);
	else if(strcmp2(flag, F_VERBOSE))
		message(0,
			"-V --verbose",
			" ",
			"Comming soon...",
		NULL);
	else if(strcmp2(flag, F_NAME   ))
		message(0,
			"-n --name",
			" ",
			"Comming soon...",
		NULL);
	else if(strcmp2(flag, F_REPLACE))
		message(0,
			"-r --replace",
			" ",
			"Comming soon...",
		NULL);
	else if(strcmp2(flag, F_LICENSE))
		message(0,
			"-l --license",
			" ",
			"Comming soon...",
		NULL);
	else
		message(0,
			"[LIM] Default (no parameters) help message",
			" ",
			"Comming soon...",
		NULL);

	exit(0);
}

void verbose(void){
	if(!g_verbose) return;

	verboseID++;

	if(verboseMsg(0, "Compaction started")) return;
	if(verboseMsg(1, "Getting content from origin")) return;
	if(verboseMsg(2, "Process finished (alpha)")) return;
	if(verboseMsg(3, "See the output in file: \"output.lim\"")) return;
}

void license(void){
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
