#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <ctype.h>
#include "tools/queue.h"
#include "tools/lim-global-variables.h"
#include "tools/string-plus.h"
#include "check-content/check-content.h"
#include "treat.h"
#include "ident-man.h"

/* NOTE:
 *******************************************************
 * if any syntax or semantic error occurr, Lim will
 * pretend that none is occurr and continue normally :D
 *******************************************************
 * vt = var_tab -> variable and table
 * f  = func[t] -> function
 *******************************************************
*/

static char *gident_nick, *gident, *gtable_key; // Global
static unsigned short layer;
static Declaration_Env *denv_bottom = NULL, *denv_top = NULL;
static const char* lua_operator_kw[3] = {"and","not","or"};
static const char* lua_boolean_kw[3] = {"false","nil","true"};

#define IS_ROOT     (layer == 0)
#define BUF_FUNC    ((IS_ROOT) ? &(lim.buffers.root.global_func) : &(lim.buffers.local.top->local_func))
#define BUF_VAR_TAB ((IS_ROOT) ? &(lim.buffers.root.global_var_tab) : &(lim.buffers.local.top->local_var_tab))
#define CTT_BUF     ((IS_ROOT) ? lim.buffers.destine_file : lim.buffers.local.top->content)
#define FORMAT(tk)  ((tk == NULL) ? "%s" : "%s%s")
#define MIN(a, b)   ((a < b) ? a : b)
#define IS_LUA_KW   (isalpha(gident[0]) && is_from_lua(gident, CKIA_LUA_KW))

void start_treatment(void){
	new_treat_env(true);
}

void finish_treatment(void){
	while(denv_top != NULL)
		print_local_declare(PLD_FORCED_END);
}

static void new_treat_env(bool is_bottom){
	Declaration_Env *new;

	new = malloc(sizeof(Declaration_Env));
	new->below = NULL;

	new->local.start_declare = !is_bottom;
	new->local.attrib_start  = false;
	new->local.expect_comma  = false;
	new->local.special_char  = false;
	new->local.compound_val  = false;

	new->local.token = LT_NULL;

	new->local.bident = NULL;
	new->local.bvalue = NULL;
	new->local.bvtail = NULL;
	
	new->local.qident = 0;
	new->local.qvalue = 0;

	if(denv_bottom == NULL){
		denv_bottom = new;
		denv_top    = new;
		return;
	}

	new->below = denv_top;
	denv_top   = new;
}

static void drop_treat_env(void){
	Declaration_Env *below;
	below = denv_top->below;
	
	qee_free_queue(denv_top->local.bident);
	qee_free_queue(denv_top->local.bvalue);
	free(denv_top);

	denv_top = below;
}

void treat_const(char *str){
	const bool is_local = (strcmp(str, "local") == 0);

	if(denv_top->local.start_declare){
		gident     = str;
		gtable_key = NULL;

		if(!denv_top->local.attrib_start)
			treat_local_declare_BEFORE_comma(NULL);
		else
			treat_local_declare_AFTER_comma(false);

	}else if(is_local){
		new_treat_env(false);
	}
}

void treat_ident(char *_ident, char *_table_key){
	gident     = _ident;
	gtable_key = _table_key;

	if(denv_top->local.start_declare){
		if(!denv_top->local.attrib_start)
			treat_local_declare_BEFORE_comma(BUF_VAR_TAB);
		else
			treat_local_declare_AFTER_comma(true);

		return;
	}
}

static void merge_compound_value(char *lident){ // Local
	char **ident, *tmp;

	ident = &(denv_top->local.bvtail->ident);
	tmp = malloc( sizeof(char) + strlen(*ident) + strlen(lident) + ((gtable_key == NULL) ? 0 : strlen(gtable_key)) );

	strcpy(tmp, *ident);
	strcat(tmp, lident);
	if(gtable_key != NULL)
		strcat(tmp, gtable_key);

	free(*ident);
	*ident = tmp;
}

static void update_local_declare(bool is_const){
	Queue **buf;
	unsigned short *qtt;
	
	
	if(!denv_top->local.attrib_start){
		buf = &(denv_top->local.bident);
		qtt = &(denv_top->local.qident);
	}else{
		buf = &(denv_top->local.bvalue);
		qtt = &(denv_top->local.qvalue);
	}

	
	const bool is_value = (*buf == denv_top->local.bvalue);
	qee_bigger_to_lower(false);
	
	if(*buf == NULL){
		*buf = qee_create(gident, gtable_key, NULL, is_const);
		(*qtt)++;

		if(is_value)
			denv_top->local.bvtail = *buf;

	}else{
		if(qee_add_item(buf, gident, gtable_key, NULL, is_const, false)){
			(*qtt)++;

			if(is_value && denv_top->local.bvtail->next != NULL)
				denv_top->local.bvtail = denv_top->local.bvtail->next;
		}
	}

	qee_bigger_to_lower(true);
}

static void print_local_declare(PLD_ID id){
	if(!denv_top->local.attrib_start){
		drop_treat_env();

		// recycle
		if(id == PLD_FAIL_CONST)
			treat_const(gident);
		else if(id == PLD_FAIL_IDENT)
			treat_ident(gident, gtable_key);

		return;
	}

	bool is_ident_buf;
	Queue *cur_buf, *cur_item, **dest_buf;
	unsigned short i;
	const unsigned short low_length = MIN(denv_top->local.qident, denv_top->local.qvalue);

	for(cur_buf = denv_top->local.bident; true; cur_buf = denv_top->local.bvalue){
		cur_item = NULL;
		is_ident_buf = (cur_buf == denv_top->local.bident);

		for(i = 0; i < low_length; i++){
			if(cur_item == NULL){
				cur_item = cur_buf;

				if(!is_ident_buf)
					fputc('=', CTT_BUF);
			}else{
				cur_item = cur_item->next;

				fputc(',', CTT_BUF);
			}

			if(is_ident_buf || !cur_item->is_const){
				if(is_ident_buf)
					gident_nick = save_ident_in_buffer(cur_item->ident, cur_item->table_key, IS_ROOT, SCOPE_IDENT, BUF_VAR_TAB);
				else
					gident_nick = get_nickname_of(cur_item->ident, IS_ROOT);

				fprintf(CTT_BUF, FORMAT(cur_item->table_key), gident_nick, cur_item->table_key);
				continue;
			}

			fprintf(CTT_BUF, FORMAT(cur_item->table_key), cur_item->ident, cur_item->table_key);
		}

		if(cur_buf == denv_top->local.bvalue)
			break;
	}


	drop_treat_env();

	// recycle
	if(id == PLD_FAIL_CONST)
		treat_const(gident);
	else if(id == PLD_FAIL_IDENT)
		treat_ident(gident, gtable_key);
}

static void treat_local_declare_BEFORE_comma(Queue **buf){
	if(buf != NULL){
		if(denv_top->local.expect_comma){
			print_local_declare(PLD_FAIL_IDENT);
			return;
		}

		denv_top->local.expect_comma = true;
		update_local_declare(false);
		return;
	}

	if(denv_top->local.expect_comma){
		if(gident[0] == '='){
			denv_top->local.attrib_start = true;
			denv_top->local.expect_comma = false;

			denv_top->local.token = LT_COMMA;
		
		}else if(gident[0] == ','){
			denv_top->local.expect_comma = false;
		
		}else{
			print_local_declare(PLD_FAIL_CONST);
		}

		return;
	}

	print_local_declare(PLD_FAIL_CONST);
}

static bool compare_token(LOCAL_TOKEN norepeat, va_list *to_copy, ...){
	if(denv_top->local.token == norepeat)
		return true;

	va_list options;

	if(to_copy == NULL)
		va_start(options, to_copy);
	else
		va_copy(options, *to_copy);

	LOCAL_TOKEN cur;
	while( (cur = va_arg(options, LOCAL_TOKEN)) != LT_NULL ){
		if(denv_top->local.token == cur){
			va_end(options);
			return true;
		}
	}

	va_end(options);
	return false;
}

static LOCAL_TOKEN get_true_token(const char lastc){
	if(strchr("=><~", lastc) != NULL)
		return LT_LOPERATOR;

	if(strchr("+-*/%^", lastc) != NULL)
		return LT_MOPERATOR;

	if(strchr("'\"", lastc) != NULL)
		return LT_STRING;

	switch(lastc){
		case '.': return LT_CONCAT;
		case '{': return LT_TABLE;
		case ',': return LT_COMMA;
		case '(': return LT_PARENO;
		case ')': return LT_PARENC;
		case '[': return LT_BRACKETO;
    	case ']': return LT_BRACKETC;
	}

	return LT_NULL;
}

static bool common_token_test(char first, char *str, LOCAL_TOKEN norepeat, LOCAL_TOKEN new_token_id, short check_comma, ...){
	va_list options;
	va_start(options, check_comma);

	if((first != 0 && gident[0] == first) || (str != NULL && strchr(str, gident[0]) != NULL)){
		if(!compare_token(norepeat, &options)){
			print_local_declare(PLD_FAIL_CONST);
			return false;
		}

		if(check_comma == 1 && denv_top->local.token == LT_COMMA)
			update_local_declare( PLD_FAIL_CONST );
		else if(first != ',')
			merge_compound_value(gident);

		const char lastc = gident[strlen(gident) - 1];
		if((first != 0 && lastc == first) || (str != NULL && strchr(str, lastc) != NULL))
			denv_top->local.token = new_token_id;
		else
			denv_top->local.token = get_true_token(lastc);

		return true;
	}

	return false;
}

static void treat_local_declare_AFTER_comma(bool is_ident){
	LOCAL_TOKEN *token;
	token = &(denv_top->local.token);

	if(is_ident){
		if(!compare_token(LT_USEORCALL, NULL, LT_COMMA, LT_MOPERATOR, LT_LOPERATOR, LT_PARENO, LT_BRACKETO, LT_NULL)){
			print_local_declare(PLD_FAIL_IDENT);
			return;
		}

		if(*token == LT_COMMA)
			update_local_declare( ((gident[0] == '_') ? PLD_FAIL_CONST : PLD_FAIL_IDENT) );
		else
			merge_compound_value( ((gident[0] == '_') ? gident : get_nickname_of(gident, IS_ROOT)) );

		*token = LT_USEORCALL;
		return;
	}

	if(isalpha(gident[0])){
		short i;

		// boolean
		for(i = 0; i < 3; i++){
			if(strcmp(gident, lua_boolean_kw[i]) == 0){

				if(!compare_token(LT_BOOLEAN, NULL, LT_COMMA, LT_PARENO, LT_LOPERATOR, LT_BRACKETO, LT_NULL)){
					print_local_declare( PLD_FAIL_CONST );
					return;
				}

			if(*token == LT_COMMA)
				update_local_declare( PLD_FAIL_CONST );
			else
				merge_compound_value(gident);

			*token = LT_BOOLEAN;
			return;
			}
		}

		// operator keyword
		for(i = 0; i < 3; i++){
			if(strcmp(gident, lua_boolean_kw[i]) == 0){

				if(!compare_token(LT_LOPERATOR, NULL, LT_USEORCALL, LT_BRACKETC, LT_BOOLEAN, LT_NUMBER, LT_PARENC, LT_STRING, LT_NULL)){
					print_local_declare( PLD_FAIL_CONST );
					return;
				}

				merge_compound_value(gident);
				*token = LT_LOPERATOR;
				return;
			}
		}

		// function -- TODO
		//if(strcmp(gident, "function") == 0){
		//	if(*token != LT_COMMA){
		//		print_local_declare( PLD_FAIL_CONST );
		//		return;
		//	}
		//	return;
		//}

		print_local_declare( PLD_FAIL_CONST );
		return;
	}

	if(isdigit(gident[0])){
		if(!compare_token(LT_NUMBER, NULL, LT_COMMA, LT_BRACKETO, LT_PARENO, LT_MOPERATOR, LT_LOPERATOR, LT_NULL)){
			print_local_declare( PLD_FAIL_CONST );
			return;
		}

		if(*token == LT_COMMA)
			update_local_declare( PLD_FAIL_CONST );
		else
			merge_compound_value(gident);

		*token = LT_NUMBER;
		return;
	}

	if(common_token_test(0,   "=><~",  LT_LOPERATOR, LT_LOPERATOR, 0, LT_BOOLEAN, LT_BRACKETC, LT_NUMBER, LT_PARENC, LT_STRING, LT_TABLE, LT_USEORCALL, LT_NULL)) return;
	if(common_token_test(0,   "+-*/%^",LT_MOPERATOR, LT_MOPERATOR, 0, LT_BRACKETC, LT_NUMBER, LT_PARENC, LT_USEORCALL, LT_NULL)) return;
	if(common_token_test('.', NULL,    LT_NULL,      LT_CONCAT,    0, LT_STRING, LT_NULL)) return;
	if(common_token_test('{', NULL,    LT_NULL,      LT_TABLE,     0, LT_BRACKETO, LT_COMMA, LT_NULL)) return;
	if(common_token_test(0,   "'\"",   LT_STRING,    LT_STRING,    1, LT_BRACKETO, LT_COMMA, LT_CONCAT, LT_LOPERATOR, LT_NULL)) return;
	if(common_token_test(',', NULL,    LT_COMMA,     LT_COMMA,     1, LT_NUMBER, LT_PARENC, LT_STRING, LT_TABLE, LT_USEORCALL, LT_NULL)) return;
	if(common_token_test('(', NULL,    LT_NULL,      LT_PARENO,    1, LT_BRACKETC, LT_BRACKETO, LT_COMMA, LT_LOPERATOR, LT_MOPERATOR, LT_NUMBER, LT_PARENC, LT_PARENO, LT_USEORCALL, LT_NULL)) return;
	if(common_token_test(')', NULL,    LT_NULL,      LT_PARENC,    1, LT_BRACKETC, LT_BRACKETO, LT_COMMA, LT_LOPERATOR, LT_MOPERATOR, LT_NUMBER, LT_PARENC, LT_PARENO, LT_USEORCALL, LT_NULL)) return;
	if(common_token_test('[', NULL,    LT_NULL,      LT_BRACKETO,  0, LT_USEORCALL, LT_NULL)) return;
	if(common_token_test(']', NULL,    LT_BRACKETO,  LT_BRACKETC,  0, LT_BOOLEAN, LT_BRACKETC, LT_NUMBER, LT_PARENC, LT_STRING, LT_TABLE, LT_USEORCALL, LT_NULL)) return;

	if(denv_top->local.start_declare)
		print_local_declare(PLD_FAIL_CONST);
}
