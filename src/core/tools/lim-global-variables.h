#ifndef CORE_TOOLS_LIM_GLOBAL_VARIABLES
#define CORE_TOOLS_LIM_GLOBAL_VARIABLES

#include <stdio.h>
#include <stdbool.h>
#include "queue.h"

typedef enum{
	LET_NULL,
	LET_FUNC,
	LET_METHOD, // like function, but with `self` included
	LET_REPEAT,
	LET_OTHER,
}Local_Env_Type;

typedef struct Local_Env{
	FILE *content;
	char *save_local_ident, *save_parameter, *save_for_loop;
	Queue *var, *func, *special;
	Local_Env_Type type;
	struct Local_Env *below;
}Local_Env;

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
		char *lib_name;
	}flags;

	struct{
		FILE *top_header;
		FILE *code_scope;
		Queue *funct_list;
		Queue *table_list;
	}header_partitions;

	struct{
		FILE *destine_file;
		FILE *scope_fpointer, *scope_faddress;

		Queue *var, *func, *lib_func;

		Queue *func_from_lua, *table_from_lua;
		Queue *func_from_header, *table_from_header;

		unsigned short lenv_quant;
		Local_Env *lenv_stack_top;
	}env_buf;
};

extern struct Lim_Global_Variables lim;

void lim_init_env(void);
void lim_free_env(void);

#endif
