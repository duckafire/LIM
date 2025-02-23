#include <stdio.h>
#include <stdlib.h>
#include "lim-global-variables.h"
#include "queue.h"

struct Lim_Global_Variables lim;

void lim_init_env(void){
	lim.files.source              = NULL;
	lim.files.destine             = NULL;
	lim.files.header_lim          = NULL;

	lim.env_buf.destine_file      = NULL;

	lim.env_buf.scope_fpointer    = NULL;
	lim.env_buf.scope_faddress    = NULL;
	
	lim.env_buf.lib_func          = NULL;
	lim.env_buf.var               = NULL;
	lim.env_buf.func              = NULL;
	lim.env_buf.func_from_lua     = NULL;
	lim.env_buf.table_from_lua    = NULL;
	lim.env_buf.func_from_header  = NULL;
	lim.env_buf.table_from_header = NULL;
	
	lim.env_buf.lenv_quant        = 0;
	lim.env_buf.lenv_stack_top    = NULL;
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


	if(lim.env_buf.destine_file != NULL)
		fclose(lim.env_buf.destine_file);

	if(lim.env_buf.scope_fpointer != NULL)
		fclose(lim.env_buf.scope_fpointer);

	if(lim.env_buf.scope_faddress != NULL)
		fclose(lim.env_buf.scope_faddress);

	qee_free_queue(lim.env_buf.lib_func);
	qee_free_queue(lim.env_buf.var);
	qee_free_queue(lim.env_buf.func);
	qee_free_queue(lim.env_buf.func_from_lua);
	qee_free_queue(lim.env_buf.table_from_lua);
	qee_free_queue(lim.env_buf.func_from_header);
	qee_free_queue(lim.env_buf.table_from_header);

	Local_Env *cur, *below;
	for(cur = lim.env_buf.lenv_stack_top; cur != NULL; cur = below){
		fclose(cur->content);

		free(cur->save_local_ident);
		free(cur->save_parameter);
		free(cur->save_for_loop);

		qee_free_queue(cur->var);
		qee_free_queue(cur->func);
		qee_free_queue(cur->special);

		below = cur->below;
		free(cur);
	}
}
