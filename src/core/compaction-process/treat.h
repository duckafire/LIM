#ifndef CORE_COMPACTION_PROCESS_TREAT
#define CORE_COMPACTION_PROCESS_TREAT

#include <stdbool.h>

void treat_const(char **tmp);
void treat_ident(char *ident, char *table_key);
static void var_tab_update_declaration(char *ident);
void declare_var_tab(void);

#endif
