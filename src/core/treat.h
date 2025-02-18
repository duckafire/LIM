#ifndef CORE_TREAT
#define CORE_TREAT

#include <stdbool.h>
#include "tools/queue.h"

typedef enum{ // Print Local Declare
	PLD_SUCCESS,
	PLD_FAIL_CONST,
	PLD_FAIL_IDENT,
	PLD_FORCED_END,
}PLD_ID;

typedef enum{
	DT_NULL = -1,
	DT_LOCAL,
	DT_FUNCTION,  // global
	DT_LIB_FUNC, // local
}DECLARE_TOKEN;

typedef struct Layer_Type{
	bool is_func_layer;
	struct Layer_Type *below;
}Layer_Type;

void finish_treatment(void);
void treat_const(char *str);
void treat_ident(char *_ident, char *_table_key);
void treat_standard_from(bool lua, char *_ident, char *_table_key, Queue **buf);
static void default_const_treatment(char *str);

static void update_layer(bool is_func);
static bool downdate_layer(void);

static void start_function_declaration(bool is_anony);
static void search_func_param_end(void);
static void pop_function_declaration(void);

static void check_if_space_is_need(char *str);
static void set_if_space_is_mandatory(char *str);

#endif
