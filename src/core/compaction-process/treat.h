#ifndef CORE_COMPACTION_PROCESS_TREAT
#define CORE_COMPACTION_PROCESS_TREAT

#include <stdbool.h>

void treat_const(char **tmp);
void treat_ident(char *ident, char *table_key);
void treat_end(void);
static void var_tab_update_declaration(char *ident);
static void declare_var_tab(void);
static bool var_tab_paren_content(char *ctt, unsigned short qtt, char fc, char ec);
static bool var_tab_open_close_paren(char fc_tmp, unsigned short *qtt, char fc, char ec, char *fs, char *es);

#endif
