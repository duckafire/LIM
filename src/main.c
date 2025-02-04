#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "args/check-and-treat.h"
#include "args/print/verbose.h"
#include "core/tools/lim-global-variables.h"
#include "core/process-master.h"
#include "core/tools/read-header-lim.h"

int main(int argc, char *argv[]){
	lim_init_env();
	atexit(lim_free_env);

	check_program_arguments(argc, argv);
	set_verbose( lim.flags.verbose );

	char *foo = NULL;
	read_header_file(&foo);
	free(foo);

	read_source_file();
	return 0;
}
