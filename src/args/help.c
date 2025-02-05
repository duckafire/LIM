#include "flags/cmp.h"
#include "flags/list.h"
#include "print/error.h"
#include "help.h"

// NOTE: all flags need to be shown in:
// HERE-0: if(flag_cmp(arg, HELP_ARG_LIST)){
// HERE-1: if(flag_cmp(arg, HELP_ARG_SYNOPSIS)){
// HERE-2: if(flag_cmp(arg, HELP_ARG_LIST_SYNOPSIS)){
// HERE-3: "end of the function:" `help_flag`

#define N "\n"

void show_help_messages(char *str){
	if(str == NULL){
		MESSAGE(
			"lim --help [keyword | flag]" N
			N
			"Display information about a keyword (these below) or a flag." N
			N
			"(h)  header   -> Display essential information about the 'header.lim'." N
			"(l)  list     -> List all Lim flags." N
			"(li) license  -> Display Lim license." N
			"(ls) list-syn -> List all Lim flags, with their synopsis." N
			"(r)  rules    -> Display Lim semantic pattern for Lua script." N
			"(s)  synopses -> Display a recommendad sinopsis to use all flags in simultaneous." N
			"'Any flag'    -> Display information about it: sinopsis; how it works." N
			);
	}

	help_with_arg(str);
	help_flag(str);

	ERROR_invalid_argument_to_flag_help(str);
}

static void help_with_arg(char *arg){
	if(flag_cmp(arg, HELP_ARG_LIST)){ // #HERE-0
		MESSAGE(
			"-v  --version" N
			"-h  --help" N
			"-V  --verbose" N
			"-n  --name" N
			"-r  --replace" N
			"-nh --no-header" N
			"-ln --lib-name" N
		);
	}
	if(flag_cmp(arg, HELP_ARG_LICENSE)){
		MESSAGE(
			"MIT License" N
			N
			"Copyright (c) 2024-2025 DuckAfire <duckafire.github.io/nest>" N
			N
			"Permission is hereby granted, free of charge, to any person obtaining a copy" N
			"of this software and associated documentation files (the 'Software'), to deal" N
			"in the Software without restriction, including without limitation the rights" N
			"to use, copy, modify, merge, publish, distribute, sublicense, and/or sell" N
			"copies of the Software, and to permit persons to whom the Software is" N
			"furnished to do so, subject to the following conditions:" N
			N
			"The above copyright notice and this permission notice shall be included in all" N
			"copies or substantial portions of the Software." N
			N
			"THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR" N
			"IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY," N
			"FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE" N
			"AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER" N
			"LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM," N
			"OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE" N
			"SOFTWARE." N
		);
	}
	if(flag_cmp(arg, HELP_ARG_RULES)){
		MESSAGE(
			"As a way of facilitating reading and interpretation of input code, some" N
			"semantic conventions were defined, named Rules. They are:" N
			N
			"1 - Only global variables, tables and functions will be added to library." N
			"    This measure was taken to avoid conflicts with identifiers extern" N
			"    to library" N
			N
			"2 - Multiple variables must not receive multiple values during themself" N
			"    declaration (`local func, tab, zero = ipairs(foo)`), only after it." N
			N
			"3 - Keywords, numbers (any type), special characteres, identifiers started" N
			"    with '_' and global identifiers will NOT be compacted." N
			N
			"4 - Global variables and tables must always be prefixed by `_G`. Otherwise," N
			"    they will not be added to library table and, eventually, they can be" N
			"    confused with local variables and tables." N
			N
			"5 - The string delimitator `[[]]` is not supported. Because the Lim final" N
			"    organization model conflict with the main feature of this delimitator," N
			"    its capacity of accept special characters, that included line feed." N
		);
	}
	if(flag_cmp(arg, HELP_ARG_HEADER)){
		MESSAGE(
			"'header.lim' is a optional file, auto searched by Lim (in current directory) when" N
			"the flag '--no-header' ('-nh') is not used." N
			N
			"If this file is found, its content will be processed to future use. Otherwise, the" N
			"compaction process will continue normally." N
			N
			"Its content can be ordenated in until four partitions, separated by \"\\n@\\n\"." N
			"These parititons are:" N
			N
			"TOP HEADER: written on top of the output file, outside the library. No compacted." N
			"CODE SCOPE: placed inside the library, below function scope. White space will be removed." N
			"TABLE LIST: will receive the same treatment that Lua standard tables." N
			"FUNCT LIST: will receive the same treatment that Lua standard functions." N
			N
			">>> EXAMPLE >>>" N
			"-- NAME: header.lim" N
			"-- VERSION: 0.0.1" N
			"-- AUTHOR: foo user" N
			"@" N
			"local function _add(a,b)return a+b end" N
			"local function _sub(a,b)return a+b end" N
			"local function _mul(a,b)return a+b end" N
			"@" N
			"foo_table_0" N
			"foo_table_1" N
			"foo_table_n" N
			"@" N
			"foo_funct_0" N
			"foo_funct_1" N
			"foo_funct_n" N
			"<<< EXAMPLE <<<" N
			N
			"The 'header.lim' will be ignored if the flag (-nh) '--no-header' is used." N
			N
			"Access the official documentation for more information:" N
			"https://github.com/duckafire/lim/blob/main/docs/o-header-lim.md" N
		);
	}
	if(flag_cmp(arg, HELP_ARG_SYNOPSIS)){ // HERE-1
		MESSAGE(
			"lim [--version | --help [keyword | flag] | <input-file>.lua [--replace]" N
			"    [--no-header] [--name <output-file-name>] [--lib-name <identifier>] ]" N
		);
	}
	if(flag_cmp(arg, HELP_ARG_LIST_SYNOPSIS)){ // HERE-2
		MESSAGE(
			"-v  --version    ->  lim --version" N
			"-h  --help       ->  lim --help [keyword | flag]" N
			"-V  --verbose    ->  lim <in>.lua --verbose" N
			"-n  --name       ->  lim <in>.lua --name <out>" N
			"-r  --replace    ->  lim <in>.lua --replace" N
			"-nh --no-header  ->  lim <in>.lua --no-header" N
			"-ln --lib-name   ->  lim <in>.lua --lib-name <lib>" N
		);
	}
}

static void help_flag(char *flag){
	if(flag_cmp(flag, FLAG_VERSION)){
		MESSAGE(
			"lim --version" N
			N
			"Display the Lim version." N
			N
			"Other arguments placed after this will be ignored." N
		);
	}
	if(flag_cmp(flag, FLAG_HELP)){
		MESSAGE(
			"lim --help [keyword | flag]" N
			N
			"Display information about one keyword (these below) or one flag." N
			N
			"header   | list  | license" N
			"list-syn | rules | synopses" N
			N
			"Other arguments placed after these will be ignored." N
		);
	}
	if(flag_cmp(flag, FLAG_VERBOSE)){
		MESSAGE(
			"lim <input>.lua --verbose" N
			N
			"It will show information related the processes that occur during the" N
			"compaction, simultaneously to it." N
			N
			"Other arguments placed after this will be ignored." N
		);
	}
	if(flag_cmp(flag, FLAG_DEST_NAME)){
		MESSAGE(
			"lim <input>.lua --name <output-file-name>" N
			N
			"Alow specify the output file name and its path. The extension (`.lim`) is" N
			"auto incremented." N
			N
			"By default, the output file name will be equal input file name, with the" N
			"extension `.lim` (instead `.lua`), and it will be placed in current directory." N
		);
	}
	if(flag_cmp(flag, FLAG_REPLACE)){
		MESSAGE(
			"lim <input>.lua --replace" N
			N
			"Specify that, if already exist a file with the same name of the output file" N
			"(inside the same destine directory), it must be replaced." N
			N
			"If this flag is not used and the condition is true, Lim will be generate an error." N
		);
	}
	if(flag_cmp(flag, FLAG_NO_HEADER)){
		MESSAGE(
			"lim <input>.lua --no-header" N
			N
			"Specific that the file 'header.lim' must be ignored." N
			N
			"Try `lim --help header` to see more about the 'header.lim'." N
		);
	}
	if(flag_cmp(flag, FLAG_LIB_NAME)){
		MESSAGE(
			"lim <input>.lua --lib-name <lib-name>" N
			N
			"Set the name of the reference table that will store the library table." N
			N
			"If it is not specified, the input file name will be adapted and used to this" N
			"finality." N
			N
			"Invalid characters will be replaced by underscore." N
		);
	}
} // HERE-3
