#include "help.h"
#include "../rules/flag-man.h"
#include "../rules/flags.h"
#include "error.h"

void show_help_messages(char *str){
	if(str == NULL){
		MESSAGE(
			"lim {-h | --help} [arg]",
			" ",
			"Print information about a specific argument, that can be a flag or one of the "
			"keywords in below:",
			"* (h) header   -> how work and how to use \"header.lim\".",
			"* (l) list     -> list all flags.",
			"* (li)license  -> program source code license.",
			"* (ls)list-syn -> list all flag with their recommended synopsis.",
			"* (r) rules    -> semantic rules to source file.",
			"* (s) synopsis -> recommended synopsis for all arguments to Lim."
		);
	}

	help_with_arg(str);
	help_flag(str);

	ERROR_invalid_argument_to_flag_help(str);
}

static void help_with_arg(char *arg){
	if(flag_cmp(arg, HELP_ARG_LIST)){
		MESSAGE(
			"-v  --version",
			"-h  --help",
			"-V  --verbose",
			"-n  --name",
			"-r  --replace",
			"-nh --no-header",
			"-us --until-stage"
		);
	}
	if(flag_cmp(arg, HELP_ARG_LICENSE)){
		MESSAGE(
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
	}
	if(flag_cmp(arg, HELP_ARG_RULES)){
		MESSAGE(
			"Work in progress"
		);
	}
	if(flag_cmp(arg, HELP_ARG_HEADER)){
		MESSAGE(
			"The \"header.lim\" is a optional file searched by Lim in start of all compaction",
			"processes. It content can be divided in four partitions:",
			" ",
			"top header: will be writed in top of the output file, outside the library.",
			"code scope: will be placed inside the library, below function scope.",
			"table list: will receive the same treatment that Lua standard tables, like \"string\"",
			"\"math\", ...",
			"func. list: will receive the same treatment that Lua standard functions, like \"select\"",
			"\"toumber\", ...",
			" ",
			"These content must be separated by string \"\\n@\\n\", like the example below:",
			" ",
			">>> FILE >>>",
			"-- Foo message",
			"@",
			"local function _foo()return\"foo\"end",
			"@",
			"foo_table_0",
			"foo_table_1",
			"foo_table_n",
			"@",
			"foo_function_0",
			"foo_function_1",
			"foo_function_n",
			"<<< FILE <<<",
			" ",
			"The \"header.lim\" will be ignored one of the flags (-nh) --no-header or (-us) --until-stage",
			"is used.",
			" ",
			"Access the official documentation for more information:",
			"https://github.com/duckafire/lim/blob/main/docs/the-header-lim.md"
		);
	}
	if(flag_cmp(arg, HELP_ARG_SYNOPSIS)){
		MESSAGE(
			"lim [ -v | -h [arg] | <in>.lua [-r] [-V] [-nh] [-us <1-5>] [-n <out>] ]"
		);
	}
	if(flag_cmp(arg, HELP_ARG_LIST_SYNOPSIS)){
		MESSAGE(
			"-v  --version     -> lim {-v | --version}",
			"-h  --help        -> lim {-h | --help} [arg]",
			"-V  --verbose     -> lim {-V | --verbose} <input>.lua",
			"-n  --name        -> lim <input>.lua {-n | --name} <output>",
			"-r  --replace     -> lim {-r | --replace} <input>.lua",
			"-nh --no-header   -> lim <source> {-nh | --no-header} <input>.lua",
			"-us --until-stage -> lim <input>.lua {-us | --until-stage} <1-5>"
		);
	}
}

static void help_flag(char *flag){
	if(flag_cmp(flag, FLAG_VERSION)){
		MESSAGE(
			"lim {-v | --version}",
			" ",
			"Print the program version"
		);
	}
	if(flag_cmp(flag, FLAG_HELP)){
		MESSAGE(
			"lim {-h | --help} [arg]",
			" ",
			"Print information about flags and other something."
		);
	}
	if(flag_cmp(flag, FLAG_VERBOSE)){
		MESSAGE(
			"lim {-V | --verbose} <input>.lua",
			" ",
			"Print information about the compaction, during this process."
		);
	}
	if(flag_cmp(flag, FLAG_DEST_NAME)){
		MESSAGE(
			"lim <input>.lua {-n | --name} <output>",
			" ",
			"Specify that the next flagument is the output file name."
		);
	}
	if(flag_cmp(flag, FLAG_REPLACE)){
		MESSAGE(
			"lim {-r | --replace} <input>.lua",
			" ",
			"Specify that, if already exist a file with the same name of the,",
			"output file must be replaced."
		);
	}
	if(flag_cmp(flag, FLAG_NO_HEADER)){
		MESSAGE(
			"lim <source> {-nh | --no-header} <input>.lua",
			" ",
			"Specific that the file \"header.lim\" must be ignored."
		);
	}
	if(flag_cmp(flag, FLAG_UNTIL_STAGE)){
		MESSAGE(
			"lim <input>.lua {-us | --until-stage} <1-5>",
			" ",
			"Specific that the compaction process must run until a specific stage."
		);
	}
}
