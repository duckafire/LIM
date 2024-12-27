#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../../../src/master/lim-global-variables.h"
#include "../../../src/args/rules/check-and-treat.h"

static void lim_free(void);
static void start_global_variables(void);
struct Lim_Global_Variables lim;

int main(int argc, char *argv[]){
start_global_variables();
atexit(lim_free);
check_program_arguments(argc, argv);
printf("Source file name: %sn", lim.source_file_name);
printf("Destine file name: %sn", lim.destine_file_name);
printf("--verbose: %dn", lim.flags.verbose);
printf("--replace: %dn", lim.flags.replace);
printf("--header file: %dn", lim.flags.header_file);
printf("--until-stage: %snn", lim.flags.until_stage);
return 0;
}

static void start_global_variables(void){
lim.source_file       = NULL;
lim.destine_file      = NULL;
lim.source_file_name  = NULL;
lim.destine_file_name = NULL;
lim.flags.verbose     = false;
lim.flags.replace     = false;
lim.flags.header_file = true;
lim.flags.until_stage = NULL;
}

static void lim_free(void){
if(lim.source_file != NULL)
fclose(lim.source_file);

if(lim.destine_file != NULL)
fclose(lim.destine_file);

free(lim.destine_file_name);
}
