#include <stdio.h>
#include <stdlib.h>
#include "lim-global-variables.h"
#include "queue.h"

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
	lim.flags.lib_name    = NULL;

	lim.header_partitions.top_header = NULL;
	lim.header_partitions.code_scope = NULL;
	lim.header_partitions.funct_list = NULL;
	lim.header_partitions.table_list = NULL;

	lim.buffers.destine_file = NULL;
	lim.buffers.root.scope_func_pointer = NULL;
	lim.buffers.root.scope_func_address = NULL;
	lim.buffers.root.scope_var_tab      = NULL;
	lim.buffers.root.global_func        = NULL;
	lim.buffers.root.global_var_tab     = NULL;
	lim.buffers.root.global_for_loop    = NULL;
	lim.buffers.root.func_from_lua      = NULL;
	lim.buffers.root.table_from_lua     = NULL;
	lim.buffers.root.func_from_header   = NULL;
	lim.buffers.root.table_from_header  = NULL;
	lim.buffers.local.top               = NULL;
	lim.buffers.local.bottom            = NULL;
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
	// copy of "argv" content
	free(lim.files.destine_name);

	// copy of "source_name", no extension,
	// or specfied by flag (-ln) "--lib-name"
	free(lim.flags.lib_name);


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

	qee_free_queue(lim.buffers.root.global_func);
	qee_free_queue(lim.buffers.root.global_var_tab);
	qee_free_queue(lim.buffers.root.global_for_loop);
	qee_free_queue(lim.buffers.root.func_from_lua);
	qee_free_queue(lim.buffers.root.table_from_lua);
	qee_free_queue(lim.buffers.root.func_from_header);
	qee_free_queue(lim.buffers.root.table_from_header);

	Func_Env_Stack *cur, *below;
	for(cur = lim.buffers.local.top; cur != NULL; cur = below){
		qee_free_queue(cur->local_func);
		qee_free_queue(cur->local_var_tab);
		qee_free_queue(cur->parameter);

		below = cur->below;
		free(cur);
	}

}
