#ifndef CORE_COMPACTION_PROCESS_CHECK_CONTENT_CONST
#define CORE_COMPACTION_PROCESS_CHECK_CONTENT_CONST

#include <stdbool.h>

bool clear_white_spaces(char *c);

bool is_number(char c, char **tmp);
bool is_string(char c, char **tmp);
bool is_table_env(char c, char **tmp);

bool is_commentary(char c);
static bool is_commentary_block(char c);
static bool clear_single_line_commentary(char c);

void is_special_char(char c, char **tmp);

#endif