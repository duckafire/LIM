#ifndef CORE_COMPACTION_PROCESS_TREAT
#define CORE_COMPACTION_PROCESS_TREAT

#include <stdbool.h>

void treat_const(char **tmp);
void treat_ident(char *ident, char *table_key);
void treat_std_hdr(char *ident, char *table_key, bool is_func, bool is_from_lua);
static void var_tab_update_declaration(char *ident);
static void declare_var_tab(void);
static void declare_function(char *ident);

#endif
