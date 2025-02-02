#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../../../src/core/tools/lim-global-variables.h"
#include "../../../src/args/rules/check-and-treat.h"

static void lim_free(void);
static void start_global_variables(void);
struct Lim_Global_Variables lim;

int main(int argc, char *argv[]){
	start_global_variables();
	atexit(lim_free);
	check_program_arguments(argc, argv);

	printf("Source file name:   %s\n", lim.files.source_name);
	printf("Destine file name:  %s\n", lim.files.destine_name);
	printf("Flags:\n");
	printf("  [-V ] Verbose messages:  %d\n",   lim.flags.verbose);
	printf("  [-r ] Replace output:    %d\n",   lim.flags.replace);
	printf("  [-nh] Header included:   %d\n",   lim.flags.header_file);
	printf("  [-ln] Library name:      %s\n",   lim.flags.lib_name);
	return 0;
}

static void start_global_variables(void){
	lim.files.source       = NULL;
	lim.files.destine      = NULL;
	lim.files.source_name  = NULL;
	lim.files.destine_name = NULL;

	lim.flags.verbose     = false;
	lim.flags.replace     = false;
	lim.flags.header_file = true;
}

static void lim_free(void){
	if(lim.files.source != NULL)
		fclose(lim.files.source);

	if(lim.files.destine != NULL)
		fclose(lim.files.destine);

	// "destine_name" store a
	// value from "argv"
	free(lim.files.destine_name);
}
