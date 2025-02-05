#include <stdlib.h>
#include "process-master.h"
#include "check.h"
#include "treat.h"

struct Args_Flags_And_Status args;

void check_program_arguments(void){
	is_it_information_flag();
	search_and_set_source_file();

	read_other_arguments();

	if(args.files_name.destine == NULL)
		set_destine_file_name( args.files_name.source );

	if(args.flags.lib_name == NULL)
		set_lib_name( args.files_name.source );

	does_dest_file_already_exist();
}

void args_init_env(unsigned int c, char *v[]){
	args.index = c - 1;
	args.value = v;
	args.content_shared = false;

	args.files_name.source  = NULL;
	args.files_name.destine = NULL;

	args.flags.verbose     = false;
	args.flags.replace     = false;
	args.flags.header_file = true;
	args.flags.lib_name    = NULL;
}

void args_free_env(void){
	// if true, they will be free
	// in "lim_free_env"
	if(args.content_shared)
		return;

	// "args.files_name.source" store
	// an address from "argv"
	free(args.files_name.destine);
	free(args.flags.lib_name);
}
