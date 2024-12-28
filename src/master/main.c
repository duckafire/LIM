#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lim-global-variables.h"
#include "../args/rules/check-and-treat.h"

static void start_global_variables(void);
static void lim_free(void);

struct Lim_Global_Variables lim;

int main(int argc, char *argv[]){
	atexit(lim_free);

	start_global_variables();

	check_program_arguments(argc, argv);
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
	lim.flags.until_stage = NULL;

	lim.header_partitions.top_header = NULL;
	lim.header_partitions.code_scope = NULL;
	lim.header_partitions.funct_list = NULL;
}

static void lim_free(void){
	if(lim.files.source != NULL)
		fclose(lim.files.source);

	if(lim.files.destine != NULL)
		fclose(lim.files.destine);


	// "destine_name" store a
	// value from "argv"
	free(lim.files.destine_name);


	if(lim.header_partitions.top_header != NULL)
		fclose(lim.header_partitions.top_header);

	if(lim.header_partitions.code_scope != NULL)
		fclose(lim.header_partitions.code_scope);

	if(lim.header_partitions.funct_list != NULL)
		fclose(lim.header_partitions.funct_list);
}
