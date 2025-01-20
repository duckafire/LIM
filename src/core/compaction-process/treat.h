#ifndef CORE_COMPACTION_PROCESS_TREAT
#define CORE_COMPACTION_PROCESS_TREAT

#include <stdbool.h>
#include "../buf-man/buf-man.h"

typedef struct Declaration_Env{
	struct{
		bool prefix, sign_found, expect_comma;
		Queue *bident, *bvalue;        // buffer
		unsigned short qident, qvalue; // quantity
		unsigned short ifunct, itable; // index
	}local;

	struct Declaration_Env *below;
}Declaration_Env;

void start_treatment(void);
void finish_treatment(void);
static void new_treat_env(bool is_bottom);
static void drop_treat_env(void);

void treat_const(char *tmp);
void treat_ident(char *ident, char *table_key);

static void build_vt_declaration(char c, char *str, bool expect_ident);
static void up_vt_declaration(char *ident, bool is_ident);
static void put_vt_declaration(void);

#endif
