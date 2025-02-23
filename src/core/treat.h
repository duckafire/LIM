#ifndef CORE_TREAT
#define CORE_TREAT

#include <stdbool.h>
#include "tools/queue.h"

typedef enum{
	DT_NULL = -1,
	DT_LOCAL,    // local foo
	DT_FUNCTION, // local function foo
	DT_LIB_FUNC, // function foo
}DECLARE_TOKEN;

void treat_const(char *str);
void treat_ident(char *_ident, char *_table_key);
void treat_standard(char *_ident, char *_table_key, Queue **buf);
static void default_const_treatment(char *str);

static void declare_function(bool is_anony);
static void search_fparam_end(void);
static void check_mandatory_space(char *str);

#endif
