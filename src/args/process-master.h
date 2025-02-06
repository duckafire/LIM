#ifndef ARGS_PROCESS_MASTER
#define ARGS_PROCESS_MASTER

#include <stdbool.h>

#define IS_DOT_LUA(str, len) \
	(len > 4                 \
	&& str[len - 4] == '.'   \
	&& str[len - 3] == 'l'   \
	&& str[len - 2] == 'u'   \
	&& str[len - 1] == 'a')

struct Args_Flags_And_Status{
	unsigned int index;
	char **value;

	bool content_shared;
	char *program_version;

	struct{
		char *source;
		char *destine;
	}files_name;
	
	struct{
		bool verbose;
		bool replace;
		bool header_file;
		char *lib_name;
	}flags;

};

extern struct Args_Flags_And_Status args;

void check_program_arguments(void);
void args_init_env(unsigned int c, char *v[], char *program_version);
void args_free_env(void);

#endif
