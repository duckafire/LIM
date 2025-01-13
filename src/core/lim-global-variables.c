#include <stdio.h>
#include <stdlib.h>
#include "lim-global-variables.h"
#include "buf-man/buf-man.h"

struct Lim_Global_Variables lim;

void lim_init_env(void){
	lim.files.source       = NULL;
	lim.files.destine      = NULL;
	lim.files.source_name  = NULL;
	lim.files.destine_name = NULL;
	lim.files.header_lim   = NULL;

	lim.flags.verbose     = false;
	lim.flags.replace     = false;
	lim.flags.header_file = true;
	lim.flags.until_stage = NULL;

	lim.header_partitions.top_header = NULL;
	lim.header_partitions.code_scope = NULL;
	lim.header_partitions.funct_list = NULL;
	lim.header_partitions.table_list = NULL;

	lim.buffers.destine_file = NULL;
	lim.buffers.source_file_line = 1;
	lim.buffers.root.scope_func_pointer = NULL;
	lim.buffers.root.scope_func_address = NULL;
	lim.buffers.root.scope_var_tab      = NULL;
	lim.buffers.root.lib_func           = NULL;
	lim.buffers.root.lib_var_tab        = NULL;
	lim.buffers.root.global_func        = NULL;
	lim.buffers.root.global_var_tab     = NULL;
}

void lim_free_env(void){
	short i;


	if(lim.files.source != NULL)
		fclose(lim.files.source);

	if(lim.files.destine != NULL)
		fclose(lim.files.destine);

	if(lim.files.header_lim != NULL)
		fclose(lim.files.header_lim);


	// "destine_name" store a
	// value from "argv"
	free(lim.files.destine_name);


	if(lim.header_partitions.top_header != NULL)
		fclose(lim.header_partitions.top_header);

	if(lim.header_partitions.code_scope != NULL)
		fclose(lim.header_partitions.code_scope);

	qee_free_queue(lim.header_partitions.funct_list);
	qee_free_queue(lim.header_partitions.table_list);


	if(lim.buffers.destine_file != NULL)
		fclose(lim.buffers.destine_file);

	if(lim.buffers.root.scope_func_pointer != NULL)
		fclose(lim.buffers.root.scope_func_pointer);

	if(lim.buffers.root.scope_func_address != NULL)
		fclose(lim.buffers.root.scope_func_address);

	if(lim.buffers.root.scope_var_tab != NULL)
		fclose(lim.buffers.root.scope_var_tab);

	qee_free_queue(lim.buffers.root.lib_func);
	qee_free_queue(lim.buffers.root.lib_var_tab);
	qee_free_queue(lim.buffers.root.global_func);
	qee_free_queue(lim.buffers.root.global_var_tab);
}
