#include "flags-in-function.h"
#include "../flags.h"
#include "../flag-man.h"
#include "../error.h"

void show_help_messages(char *str){
	if(str == NULL)
		MESSAGE(
			"lim {-h | --help} [arg]",
			" ",
			"Print informations about a specific argument, that can be a flag or one of the"
			"keywords in below:",
			"* (h) header   -> how work and how to use \"header.lim\".",
			"* (l) list     -> list all flags.",
			"* (li)license  -> program source code license.",
			"* (ls)list-syn -> list all flag with their recommended synopsis.",
			"* (r) rules    -> semantic rules to source file.",
			"* (s) synopsis -> recommended synopsis for all arguments to Lim."
		);

	help_with_arg(str);
	help_flag(str);

	ERROR_invalid_argument_to_flag_help(str);
}

static void help_with_arg(char *arg){
	if(flag_cmp(arg, HELP_ARG_LIST))
		MESSAGE(0,
			"-v  --version",
			"-h  --help",
			"-V  --verbose",
			"-n  --name",
			"-r  --replace",
			"-nh --no-header",
			"-us --until-stage"
		);
	if(flag_cmp(arg, HELP_ARG_LICENSE))
		MESSAGE(0,
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
			"SOFTWARE."
		);
	if(flag_cmp(arg, HELP_ARG_RULES))
		MESSAGE(0,
			"Compaction rules (work in progress):",
			" ",
			"1 - Global variables and tables must ALWAYS to be prefixed by \"_G.\".",
			"2 - Global variables, tables and functions will be added to the library table.",
			"3 - Variables, tables and functions declared inside aligned functions must not have",
			"    the same name.",
			"4 - \"_\" is a reservated identifier to appoint the library table, because of this,",
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
			"    line feed)."
		);
	if(flag_cmp(arg, HELP_ARG_HEADER))
		MESSAGE(0,
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
			"rectangleBoard"
		);
	if(flag_cmp(arg, HELP_ARG_SYNOPSIS))
		MESSAGE(0,
			"lim [-v] [-h [help-arg]] [-V] [-r] <src-name> [-nh] [-us <1 | 2 | 3 | 4>] [-n <dest-name>]"
		);
	if(flag_cmp(arg, HELP_ARG_LIST_SYNOPSIS))
		MESSAGE(0,
			"-v  --version     -> lim {-v | --version}",
			"-h  --help        -> lim {-h | --help} [arg]",
			"-V  --verbose     -> lim {-V | --verbose} <source-name>",
			"-n  --name        -> lim <source-name> {-n | --name} <destine-name>",
			"-r  --replace     -> lim {-r | --replace} <source-name>",
			"-nh --no-header   -> lim <source> {-nh | --no-header} <source-name>",
			"-us --until-stage -> lim <source-name> {-us | --until-stage} <1 | 2 | 3 | 4>"
		);
}

static void help_flag(char *flag){
	if(flag_cmp(flag, FLAG_VERSION))
		message(0,
			"lim {-v | --version}",
			" ",
			"Print the program version"
		);
	if(flag_cmp(flag, FLAG_HELP))
		message(0,
			"lim {-h | --help} [flag]",
			" ",
			"Print informations about flags and other something."
		);
	if(flag_cmp(flag, FLAG_VERBOSE))
		message(0,
			"lim {-V | --verbose} <source-name>",
			" ",
			"Print informations about the compaction, during this process."
		);
	if(flag_cmp(flag, FLAG_DEST_NAME))
		message(0,
			"lim <source-name> {-n | --name} <destine-name>",
			" ",
			"Specify that the next flagument is the output file name."
		);
	if(flag_cmp(flag, FLAG_REPLACE))
		message(0,
			"lim {-r | --replace} <source-name>",
			" ",
			"Specify that, if already exist a file with the same name of the",
			"output file, it must be replaced."
		);
	if(flag_cmp(flag, FLAG_NO_HEADER))
		message(0,
			"lim <source> {-nh | --no-header} <source-name>",
			" ",
			"Specific that the file \"header.lim\" must be ignored."
		);
	if(flag_cmp(flag, FLAG_UNTIL_STAGE))
		message(0,
			"lim <source-name> {-us | --until-stage} <1 | 2 | 3 | 4>",
			" ",
			"Specific that the compaction process must run until a specific stage."
		);
}
