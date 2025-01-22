#ifndef CORE_COMPACTION_PROCESS_TREAT
#define CORE_COMPACTION_PROCESS_TREAT

#include <stdbool.h>
#include "tools/queue.h"

typedef struct Declaration_Env{
	struct{
		bool prefix, sign_found, expect_comma, special_cvalue;
		Queue *bident, *bvalue, *bvtail; // buffer
		unsigned short qident, qvalue;   // quantity
		unsigned short ifunct, itable;   // index
	}local;

	struct Declaration_Env *below;
}Declaration_Env;

void start_treatment(void);
void finish_treatment(void);
static void new_treat_env(bool is_bottom);
static void drop_treat_env(void);

void treat_const(char *tmp);
void treat_ident(char *ident, char *table_key);

static bool is_special_cvalue(void);
static void merge_cur_str_with_bvtail(bool special_cvalue, bool expect_comma);
static void vt_comma_or_cvalue(void);
static void build_vt_declaration(bool expect_ident);
static void up_vt_declaration(bool is_const);
static void put_vt_declaration(void);

#endif
