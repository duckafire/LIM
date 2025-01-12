#ifndef CORE_COMPACTION_PROCESS_CHECK_CONTENT_IDENT
#define CORE_COMPACTION_PROCESS_CHECK_CONTENT_IDENT

#include <stdbool.h>

bool is_identifier(char c, char **tmp);
bool is_lua_keyword(char *ident); // TODO: it will be static

#endif
