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

	if(code < 0 || code > 0){
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
		"local str, car, foo = \"Hello world\", 29",
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
