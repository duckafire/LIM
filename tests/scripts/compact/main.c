#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "../debug-common.h"
#include "../../../src/core/tools/lim-global-variables.h"
#include "../../../src/core/process-master.h"

FILE* write_and_print_source_file(short code);

#define SOURCE_LUA "./scripts/.tmp/source.lua"

int main(int argc, char *argv[]){
	CHECK_PROGRAM_ARG_QUANTITY

	short code = (short)strtol(argv[1], NULL, 10);

	lim_init_env();
	atexit(lim_free_env);

	if(code < 0 || code > 100){
		INVALID_ARG_TO_MAIN("values between 0-?");
	}

	lim.buffers.destine_file = tmpfile();
	lim.buffers.root.scope_func_pointer = tmpfile();
	lim.buffers.root.scope_func_address = tmpfile();
	lim.buffers.root.scope_var_tab = tmpfile();

	lim.files.source = write_and_print_source_file(code);
	puts("[ SOURCE.LIM ]");
	read_source_file();
	puts("\n[ SOURCE.LIM ]\n");

	return 0;
}

FILE* write_and_print_source_file(short code){
	char c;
	FILE *file;
	const char *content[] = {
		"local A, B = a[b]+((17 * vrum ) / 19 - _foo(arg), \"str\" .. 'c' -- comment",
		"local a, b, c, d, e = 1 * 1.0 + (1 - 1.00) - A.C / A ^ 10 // A[B]",
		"local a, b, c, d, e = 1 - 1.0 - (1 - 1.00) / A - 10 - A[B]",
		"local strstr, c4r_, f00 = 'Hello world!', 29 + 1",
		"local str = 'Hello world!', 29",
		"local strstr, c4r_, f00 = 'Hello world!', 29, false",
		"local strstr, c4r_, f00 = f00, 29",
		"local strstr, c4r_, f00",
		"local strstr, car = 'Hello world!', 29",
		"local str = 'Hello world!'",
		"local strstr, c4r_, f00 = 'Hello world!', 29, c4r_",
		"local str = 'Hello world!', 29",
		"local strstr, c4r_, f00 = 'Hello world', 29, false",
		"local strstr, c4r_, f00 = 'Hello world', 29",
		"local strstr, c4r_, f00",
		"local strstr, car = 'Hello world', 29",
		"local str = 'Hello world'",
		"local 0 = 12",
		"local str, 0 = 'Hello world!'",
		"local strstr, f00 = false, strstr, 14, 29",
		"local strstr, f00, it, n = false, true, 14, 29",
		"local strstr, f00, it, 0 = vrum, true, 14, 29",
		"local strstr, f00, 0, n = false, true, 14, 29",
		"local str, f00, 0, n = vrum, 13 ",
		"local f00, n, n = 0xabc",
		"local 0, n = 'cow', cow",
		"local strstr, f00, 0, n = false, true, 14, 29",
	};
	

	file = fopen(SOURCE_LUA, "w");
	if(errno == ENOENT){
		puts("Impossible find " SOURCE_LUA "\nGo to ./tests/\n");
		exit(1);
	}

	fputs(content[code], file);
	fclose(file);
	file = fopen(SOURCE_LUA, "r");


	puts("[ source.lua ]");

	while((c = fgetc(file)) != EOF)
		putchar(c);

	puts("\n[ source.lua ]\n");
	fseek(file, 0, SEEK_SET);


	return file;
}
