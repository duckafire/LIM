#include <stdio.h>
#include <stdlib.h>
#include "../debug-common.h"
#include "../../../src/core/lim-global-variables.h"
#include "../../../src/core/compaction-process/process-master.h"

FILE* write_and_print_source_file(short code);

int main(int argc, char *argv[]){
	CHECK_PROGRAM_ARG_QUANTITY

	short code = (short)strtol(argv[1], NULL, 10);

	lim_init_env();
	atexit(lim_free_env);

	if(code < 0 || code > 20){
		INVALID_ARG_TO_MAIN("values between 0-?");
	}

	lim.buffers.destine_file = tmpfile();
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
		"local strstr, c4r_, f00 = 'Hello world!', 29",
		"local str = 'Hello world!', 29",
		"local strstr, c4r_, f00 = 'Hello world!', 29, false",
		"local strstr, c4r_, f00 = 'Hello world!', 29",
		"local strstr, c4r_, f00",
		"local strstr, car = 'Hello world!', 29",
		"local str = 'Hello world!'",
		"local strstr, c4r_, f00 = 'Hello world!', 29",
		"local str = 'Hello world!', 29",
		"local strstr, c4r_, f00 = 'Hello world', 29, false",
		"local strstr, c4r_, f00 = 'Hello world', 29",
		"local strstr, c4r_, f00",
		"local strstr, car = 'Hello world', 29",
		"local str = 'Hello world'",
		"local 0 = 12",
		"local str, 0 = 'Hello world!'",
		"local strstr, f00 = false, true, 14, 29",
		"local strstr, f00, it, n = false, true, 14, 29",
		"local strstr, f00, it, 0 = false, true, 14, 29",
		"local strstr, f00, 0, n = false, true, 14, 29",
	};
	

	file = fopen("source.lua", "w");
	fputs(content[code], file);
	fclose(file);
	file = fopen("source.lua", "r");


	puts("[ source.lua ]");

	while((c = fgetc(file)) != EOF)
		putchar(c);

	puts("\n[ source.lua ]\n");
	fseek(file, 0, SEEK_SET);


	return file;
}
