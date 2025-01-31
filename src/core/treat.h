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

typedef enum{
	LT_NULL = -1,
	LT_BOOLEAN,
	LT_BRACKETC,
	LT_BRACKETO,
	LT_COMMA,
	LT_CONCAT,
	LT_FUNC_END,
	LT_FUNC_START,
	LT_LOPERATOR,
	LT_MOPERATOR,
	LT_NUMBER,
	LT_PARENC,
	LT_PARENO,
	LT_STRING,
	LT_TABLE,
	LT_USEORCALL,
}LOCAL_TOKEN;

// NOTE:
// local a = function()
// // local b = function()
// // // local c = function() ...
typedef struct Stack_Env{
	bool start_declare; // local
	bool in_ident_decl; // foo = function() end
	bool spc_is_mandat; // before chain
	bool in_root_envir; // save IS_ROOT state
	bool attrib_start;  // =
	bool expect_comma;  // ,
	LOCAL_TOKEN token;
	Queue *bident, *bvalue, *bvtail; // buffer
	unsigned short qident, qvalue;   // quantity
	struct Stack_Env *below;
}Stack_Env;

void finish_treatment(void);
void treat_const(char *str);
void treat_ident(char *_ident, char *_table_key);

static void update_layer(bool is_func);
static bool downdate_layer(void);

static void treat_local_declare_BEFORE_comma(Queue **buf);
static bool compare_token(LOCAL_TOKEN norepeat, va_list *to_copy, ...);
static LOCAL_TOKEN get_true_token(const char lastc);
static bool common_token_test(char first, char *str, LOCAL_TOKEN norepeat, LOCAL_TOKEN new_token_id, short check_comma, ...);
static void treat_local_declare_AFTER_comma(bool is_ident);

static void new_var_tab_declare_env(bool start, bool in_ident_decl);
static void drop_var_tab_declare_env(bool force);
static void merge_compound_value(char *lident);
static void update_local_declare(bool is_const);
static void print_local_declare(PLD_ID id);

static void start_function_declaration(bool is_anony);
static void search_func_param_end(void);
static bool pop_function_declaration(void);

static void check_if_space_is_need(char *str);
static void set_if_space_is_mandatory(char *str);

#endif
