#ifndef INTERFACES_LIM_GLOBAL_VARIABLES
#define INTERFACES_LIM_GLOBAL_VARIABLES

#include <stdio.h>
#include <stdbool.h>

struct Lim_Global_Variables{
	FILE *source_file;
	FILE *destine_file;

	char *source_file_name;
	char *destine_file_name;
	
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
