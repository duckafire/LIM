#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "args/process-master.h"
#include "core/tools/lim-global-variables.h"
#include "core/process-master.h"
#include "core/tools/read-header-lim.h"

int main(int argc, char *argv[]){
	lim_init_env();
	atexit(lim_free_env);


	args_init_env(argc, argv);
	atexit(args_free_env);

	check_program_arguments();

	lim.files.source_name  = args.files_name.source;
	lim.files.destine_name = args.files_name.destine;

	lim.flags.verbose     = args.flags.verbose;
	lim.flags.replace     = args.flags.replace;
	lim.flags.header_file = args.flags.header_file;
	lim.flags.lib_name    = args.flags.lib_name;

	args.content_shared = true;


	char *foo = NULL;
	read_header_file(&foo);
	free(foo);


	read_source_file();
	return 0;
}
