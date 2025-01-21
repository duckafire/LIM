#ifndef CORE_TOOLS_LIM_GLOBAL_VARIABLES
#define CORE_TOOLS_LIM_GLOBAL_VARIABLES

#include <stdio.h>
#include <stdbool.h>
#include "queue.h"

typedef struct Func_Env_Stack{
	FILE *content;
	FILE *scope_var_tab;
	Queue *local_func;
	Queue *local_var_tab;
	Queue *parameter;
	struct Func_Env_Stack *below;
}Func_Env_Stack;

struct Lim_Global_Variables{
	struct{
		FILE *source;
		FILE *destine;
		char *source_name;
		char *destine_name;
		FILE *header_lim;
	}files;
	
	struct{
		bool verbose;
		bool replace;
		bool header_file;
		char *until_stage;
	}flags;

	struct{
		FILE *top_header;
		FILE *code_scope;
		Queue *funct_list;
		Queue *table_list;
	}header_partitions;

	struct{
		FILE *destine_file;
		unsigned long source_file_line;

		struct{
			FILE *scope_func_pointer, *scope_func_address, *scope_var_tab;
			Queue *global_func, *global_var_tab;
			Queue *func_from_lua, *table_from_lua;
			Queue *func_from_header, *table_from_header;
		}root;

		struct{
			// Queue *local_func, *local_var_tab, *parameter;
			Func_Env_Stack *top;
			Func_Env_Stack *bottom;
		}local;
	}buffers;
};

extern struct Lim_Global_Variables lim;

void lim_init_env(void);
void lim_free_env(void);

#endif
