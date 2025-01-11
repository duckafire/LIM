#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "core/lim-global-variables.h"
#include "args/rules/check-and-treat.h"
#include "core/buf-man/buf-man.h"

// TODO:
// check lua sintaxe with lua-c api
// (before and after the compaction)

int main(int argc, char *argv[]){
	lim_init_env();
	atexit(lim_free_env);

	check_program_arguments(argc, argv);
	return 0;
}
