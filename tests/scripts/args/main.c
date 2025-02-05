#include <stdio.h>
#include <stdlib.h>
#include "../../../src/args/process-master.h"

int main(int argc, char *argv[]){
	args_init_env(argc, argv);
	atexit(args_free_env);

	check_program_arguments();

	printf("Source file name:  %s\n", args.files_name.source);
	printf("Destine file name: %s\n", args.files_name.destine);
	printf("Flags:\n");
	printf("  [-V ] Verbose messages: %d\n", args.flags.verbose);
	printf("  [-r ] Replace output:   %d\n", args.flags.replace);
	printf("  [-nh] Header included:  %d\n", args.flags.header_file);
	printf("  [-ln] Library name:     %s\n", args.flags.lib_name);
	putchar('\n');
	return 0;
}
