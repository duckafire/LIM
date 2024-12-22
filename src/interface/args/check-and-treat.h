#ifndef INTERFACE_ARGS_CHECK_AND_TREAT
#define INTERFACE_ARGS_CHECK_AND_TREAT

#include <stdio.h>
#include <stdbool.h>

void check_program_arguments(int c, char *v[]);
static void is_it_information_flag(void);
static void search_and_set_source_file(void);
static bool read_other_arguments(void);
static void set_destine_file_name(const char *src);

#endif
