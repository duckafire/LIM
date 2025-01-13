#ifndef MASTER_LIM_GLOBAL_VARIABLES
#define MASTER_LIM_GLOBAL_VARIABLES

#include <stdio.h>
#include <stdbool.h>
#include "buf-man/buf-man.h"

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
		}root;

		struct{
			FILE *scope_func_pointer, *scope_func_address, *scope_var_tab;
			Queue *global_func, *global_var_tab;
			Queue *its_parameter, *aligned_parameter;
		}local;
	}buffers;
};

extern struct Lim_Global_Variables lim;

void lim_init_env(void);
void lim_free_env(void);

#endif
