#ifndef MASTER_LIM_GLOBAL_VARIABLES
#define MASTER_LIM_GLOBAL_VARIABLES

#include <stdio.h>
#include <stdbool.h>

struct Lim_Global_Variables{
	struct{
		FILE *source;
		FILE *destine;
		char *source_name;
		char *destine_name;
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
		FILE *funct_list;
	}header_partitions;
};

extern struct Lim_Global_Variables lim;

#endif
