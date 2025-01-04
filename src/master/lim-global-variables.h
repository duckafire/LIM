#ifndef MASTER_LIM_GLOBAL_VARIABLES
#define MASTER_LIM_GLOBAL_VARIABLES

#include <stdio.h>
#include <stdbool.h>
#include "../core/buf-man/buf-man.h"

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
};

extern struct Lim_Global_Variables lim;

#endif
