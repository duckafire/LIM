#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lim-global-variables.h"
#include "args/check-and-treat.h"

static void lim_free(void);
struct Lim_Global_Variables lim;

int main(int argc, char *argv[]){
	lim.source_file       = NULL;
	lim.destine_file      = NULL;
	lim.source_file_name  = NULL;
	lim.destine_file_name = NULL;

	lim.flags.verbose     = false;
	lim.flags.replace     = false;
	lim.flags.header_file = true;
	lim.flags.until_stage = NULL;

	atexit(lim_free);

	check_program_arguments(argc, argv);
	return 0;
}

static void lim_free(void){
	if(lim.source_file != NULL)
		fclose(lim.source_file);

	if(lim.destine_file != NULL)
		fclose(lim.destine_file);

	free(lim.source_file_name);
	free(lim.destine_file_name);
}
