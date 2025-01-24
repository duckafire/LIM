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
	LT_NULL = -1,
	LT_BOOLEAN,
	LT_BRACKETC,
	LT_BRACKETO,
	LT_COMMA,
	LT_CONCAT,
	LT_LOPERATOR,
	LT_MOPERATOR,
	LT_NUMBER,
	LT_PARENC,
	LT_PARENO,
	LT_STRING,
	LT_TABLE,
	LT_USEORCALL,
}LOCAL_TOKEN;

typedef struct Declaration_Env{
	struct{
		bool start_declare; // local
		bool attrib_start;  // =
		bool expect_comma;
		bool compound_val;
		bool special_char;  // + - * / [...]
		LOCAL_TOKEN token;
		Queue *bident, *bvalue, *bvtail; // buffer
		unsigned short qident, qvalue;   // quantity
	}local;
	
	struct Declaration_Env *below;
}Declaration_Env;

void start_treatment(void);
void finish_treatment(void);
static void new_treat_env(bool is_bottom);
static void drop_treat_env(void);
void treat_const(char *str);
void treat_ident(char *_ident, char *_table_key);
static void merge_compound_value(char *lident);
static void update_local_declare(bool is_const);
static void print_local_declare(PLD_ID id);
static void treat_local_declare_BEFORE_comma(Queue **buf);
static bool compare_token(LOCAL_TOKEN norepeat, va_list *to_copy, ...);
static LOCAL_TOKEN get_true_token(const char lastc);
static bool common_token_test(char first, char *str, LOCAL_TOKEN norepeat, LOCAL_TOKEN new_token_id, short check_comma, ...);
static void treat_local_declare_AFTER_comma(bool is_ident);

#endif
