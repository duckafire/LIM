#ifndef ARGS_RULES_CHECK_AND_TREAT
#define ARGS_RULES_CHECK_AND_TREAT

void check_program_arguments(int c, char *v[]);
static void is_it_information_flag(void);
static void search_and_set_source_file(void);
static void read_other_arguments(void);
static void set_destine_file_name(const char *src);
static void does_dest_file_already_exist(void);
static void set_lib_name(const char *src);

#endif
