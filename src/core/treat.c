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
*/

static char *gident_nick, *gident, *gtable_key; // Global
static unsigned short layer = 0;
static const char* lua_operator_kw[3] = {"and","not","or"};
static const char* lua_boolean_kw[3] = {"false","nil","true"};
static char *anony_func_to_local_declare = NULL;
static DECLARE_TOKEN dtoken = DT_NULL;
static Stack_Env *locald = NULL; // LOCAL Declaration

struct{ bool start_declare; bool parameter_end; }functd = {false, false};

#define IS_ROOT     (layer == 0)
#define BUF_FUNC    ((IS_ROOT) ? &(lim.buffers.root.global_func) : &(lim.buffers.local.top->local_func))
#define BUF_VAR_TAB ((IS_ROOT) ? &(lim.buffers.root.global_var_tab) : &(lim.buffers.local.top->local_var_tab))
#define CTT_BUF     ((IS_ROOT) ? lim.buffers.destine_file : lim.buffers.local.top->content)
#define FORMAT(tk)  ((tk == NULL) ? "%s" : "%s%s")
#define MIN(a, b)   ((a < b) ? a : b)
#define IS_LUA_KW   (isalpha(gident[0]) && is_from_lua(gident, CKIA_LUA_KW))
#define LOCALD_ON   (locald != NULL && locald->start_declare)

void finish_treatment(void){
	while(locald != NULL){
		print_local_declare(PLD_FORCED_END);
		drop_var_tab_declare_env();
	}
}

void treat_const(char *str){
	const bool is_local = (strcmp(str, "local") == 0);
	const bool is_funct = (strcmp(str, "function") == 0);
	const bool is_func_param_declare = (functd.start_declare && !functd.parameter_end);

	gident     = str;
	gtable_key = NULL;

	if(!is_func_param_declare && layer > 0){
		if(strcmp(str, "if") == 0 || strcmp(str, "do") == 0)
			layer++;
		else if(strcmp(str, "end") == 0)
			layer--;

		if(layer == 0){
			restart_local_parameter_nicknames();
			const bool is_anony = (locald == NULL) ? false : locald->is_anony_func;

			if(is_anony){
				drop_var_tab_declare_env();

				if(LOCALD_ON)
					locald->token = LT_FUNC_END;
			}

			drop_local_environment( ((locald != NULL && is_anony) ? &anony_func_to_local_declare : NULL) );

			if(anony_func_to_local_declare != NULL){
				gident = anony_func_to_local_declare;
				update_local_declare(true);
				merge_compound_value("end");
				string_set(&anony_func_to_local_declare, STR_END);
				return;
			}
		}
	}

	if(is_func_param_declare){
		fprintf(CTT_BUF, "%s", gident);
		search_func_param_end();

	}else if(dtoken == DT_LIB_FUNC && gident[0] == '('){
		start_function_declaration(true);
		search_func_param_end();

	}else if(dtoken == DT_LOCAL){
		if(is_funct){
			dtoken = DT_FUNCTION;
		}else{
			fprintf(CTT_BUF, "local");
			dtoken = DT_NULL;
		}

	}else if(LOCALD_ON){
		if(!locald->attrib_start)
			treat_local_declare_BEFORE_comma(NULL);
		else
			treat_local_declare_AFTER_comma(false);

	}else if(is_funct){
		dtoken = DT_LIB_FUNC;

	}else if(is_local){
		dtoken = DT_LOCAL;

	}else if(dtoken != DT_NULL){
		dtoken = DT_NULL;
		fprintf(CTT_BUF, "%s", str);

	}else if(dtoken == DT_NULL){
		fprintf(CTT_BUF, "%s", str);
	}
}

void treat_ident(char *_ident, char *_table_key){
	gident     = _ident;
	gtable_key = _table_key;

	if(dtoken == DT_LOCAL){
		man_var_tab_declare_env(true, true, false);
		dtoken = DT_NULL;

	}else if(dtoken == DT_FUNCTION || dtoken == DT_LIB_FUNC){
		if(dtoken == DT_FUNCTION)
			gident = save_ident_in_buffer(gident, gtable_key, IS_ROOT, SCOPE_IDENT, BUF_VAR_TAB);

		start_function_declaration(false);
		return;
	}

	if(functd.start_declare && !functd.parameter_end){
		gident_nick = save_ident_in_buffer(gident, gtable_key, IS_ROOT, SCOPE_PARAM, &(lim.buffers.local.top->parameter));
		fprintf(CTT_BUF, FORMAT(gtable_key), gident_nick, gtable_key);
		return;

	}else if(LOCALD_ON){
		if(locald->token == LT_FUNC_START){
			print_local_declare(PLD_FORCED_END);
			start_function_declaration(false);
			return;
		}

		if(!locald->attrib_start)
			treat_local_declare_BEFORE_comma(BUF_VAR_TAB);
		else
			treat_local_declare_AFTER_comma(true);

		return;
	}
}


static void treat_local_declare_BEFORE_comma(Queue **buf){
	if(buf != NULL){
		if(locald->expect_comma){
			print_local_declare(PLD_FAIL_IDENT);
			return;
		}

		locald->expect_comma = true;
		update_local_declare(false);
		return;
	}

	if(locald->expect_comma){
		if(gident[0] == '='){
			locald->attrib_start = true;
			locald->expect_comma = false;

			locald->token = LT_COMMA;
		
		}else if(gident[0] == ','){
			locald->expect_comma = false;
		
		}else{
			print_local_declare(PLD_FAIL_CONST);
		}

		return;
	}

	print_local_declare(PLD_FAIL_CONST);
}

static bool compare_token(LOCAL_TOKEN norepeat, va_list *to_copy, ...){
	if(locald->token == norepeat)
		return true;

	va_list options;

	if(to_copy == NULL)
		va_start(options, to_copy);
	else
		va_copy(options, *to_copy);

	LOCAL_TOKEN cur;
	while( (cur = va_arg(options, LOCAL_TOKEN)) != LT_NULL ){
		if(locald->token == cur){
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

		if(check_comma == 1 && locald->token == LT_COMMA)
			update_local_declare( PLD_FAIL_CONST );
		else if(first != ',')
			merge_compound_value(gident);

		const char lastc = gident[strlen(gident) - 1];
		if((first != 0 && lastc == first) || (str != NULL && strchr(str, lastc) != NULL))
			locald->token = new_token_id;
		else
			locald->token = get_true_token(lastc);

		return true;
	}

	return false;
}

static void treat_local_declare_AFTER_comma(bool is_ident){
	LOCAL_TOKEN *token;
	token = &(locald->token);

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

		// function
		if(strcmp(gident, "function") == 0){
			if(*token != LT_COMMA){
				print_local_declare( PLD_FAIL_CONST );
				return;
			}

			*token = LT_FUNC_START;
			return;
		}

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

	if(*token == LT_FUNC_START){
		if(gident[0] != '('){
			print_local_declare(PLD_FAIL_CONST);
			return;
		}

		start_function_declaration(true);

		for(short i = 1; gident[i] != '\0'; i++){
			if(gident[i] == ')'){
				functd.parameter_end = true;
				break;
			}
		}

		return;
	}

	if(common_token_test(0,"=><~",  LT_LOPERATOR, LT_LOPERATOR, 0, LT_BOOLEAN, LT_BRACKETC, LT_NUMBER, LT_PARENC, LT_STRING, LT_TABLE, LT_USEORCALL, LT_NULL)) return;
	if(common_token_test(0,"+-*/%^",LT_MOPERATOR, LT_MOPERATOR, 0, LT_BRACKETC, LT_NUMBER, LT_PARENC, LT_USEORCALL, LT_NULL)) return;
	if(common_token_test('.', NULL, LT_NULL,      LT_CONCAT,    0, LT_STRING, LT_NULL)) return;
	if(common_token_test('{', NULL, LT_NULL,      LT_TABLE,     0, LT_BRACKETO, LT_COMMA, LT_NULL)) return;
	if(common_token_test(0,   "'\"",LT_STRING,    LT_STRING,    1, LT_BRACKETO, LT_COMMA, LT_CONCAT, LT_LOPERATOR, LT_NULL)) return;
	if(common_token_test(',', NULL, LT_COMMA,     LT_COMMA,     1, LT_BOOLEAN, LT_FUNC_END, LT_NUMBER, LT_PARENC, LT_STRING, LT_TABLE, LT_USEORCALL, LT_NULL)) return;
	if(common_token_test('(', NULL, LT_NULL,      LT_PARENO,    1, LT_BRACKETC, LT_BRACKETO, LT_COMMA, LT_LOPERATOR, LT_MOPERATOR, LT_NUMBER, LT_PARENC, LT_PARENO, LT_USEORCALL, LT_NULL)) return;
	if(common_token_test(')', NULL, LT_NULL,      LT_PARENC,    1, LT_BRACKETC, LT_BRACKETO, LT_COMMA, LT_LOPERATOR, LT_MOPERATOR, LT_NUMBER, LT_PARENC, LT_PARENO, LT_USEORCALL, LT_NULL)) return;
	if(common_token_test('[', NULL, LT_NULL,      LT_BRACKETO,  0, LT_USEORCALL, LT_NULL)) return;
	if(common_token_test(']', NULL, LT_BRACKETO,  LT_BRACKETC,  0, LT_BOOLEAN, LT_BRACKETC, LT_NUMBER, LT_PARENC, LT_STRING, LT_TABLE, LT_USEORCALL, LT_NULL)) return;

	if(LOCALD_ON)
		print_local_declare(PLD_FAIL_CONST);
}


static void man_var_tab_declare_env(bool new, bool startl, bool is_anony){
	if(new){
		Stack_Env *new;
		new = malloc(sizeof(Stack_Env));

		if(locald == NULL){
			locald = new;
			locald->below = NULL;

		}else{
			new->below = locald;
			locald       = new;
		}

	}else if(locald == NULL){
		return;
	
	}else{
		qee_free_queue(locald->bident);
		qee_free_queue(locald->bvalue);
	}

	// set new / reset
	locald->start_declare = startl;
	locald->is_anony_func = is_anony;
	locald->attrib_start = false;
	locald->expect_comma = false;

	locald->token = LT_NULL;

	locald->bident = NULL;
	locald->bvalue = NULL;
	locald->bvtail = NULL;

	locald->qident = 0;
	locald->qvalue = 0;
}

static void drop_var_tab_declare_env(void){
	Stack_Env *below;
	below = locald->below;
	
	free(locald);
	locald = below;
}

static void merge_compound_value(char *lident){ // Local
	char **ident, *tmp;

	ident = &(locald->bvtail->ident);
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
	
	
	if(!locald->attrib_start){
		buf = &(locald->bident);
		qtt = &(locald->qident);
	}else{
		buf = &(locald->bvalue);
		qtt = &(locald->qvalue);
	}

	
	const bool is_value = (*buf == locald->bvalue);
	qee_bigger_to_lower(false);
	
	if(*buf == NULL){
		*buf = qee_create(gident, gtable_key, NULL, is_const);
		(*qtt)++;

		if(is_value)
			locald->bvtail = *buf;

	}else{
		if(qee_add_item(buf, gident, gtable_key, NULL, is_const, ((*buf == locald->bvalue) ? QEE_INSERT : QEE_DROP))){
			(*qtt)++;

			if(is_value && locald->bvtail->next != NULL)
				locald->bvtail = locald->bvtail->next;
		}
	}

	qee_bigger_to_lower(true);
}

static void print_local_declare(PLD_ID id){
	if(!locald->attrib_start){
		man_var_tab_declare_env(false, false, false);

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
	const unsigned short low_length = MIN(locald->qident, locald->qvalue);

	for(cur_buf = locald->bident; true; cur_buf = locald->bvalue){
		cur_item = NULL;
		is_ident_buf = (cur_buf == locald->bident);

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

		if(cur_buf == locald->bvalue)
			break;
	}


	man_var_tab_declare_env(false, false, false);

	// recycle
	if(id == PLD_FAIL_CONST)
		treat_const(gident);
	else if(id == PLD_FAIL_IDENT)
		treat_ident(gident, gtable_key);
}


static void start_function_declaration(bool is_anony){
	layer++;

	functd.start_declare = true;
	functd.parameter_end = false;
	new_local_environment();

	if(is_anony){
		fprintf(CTT_BUF, "function%s", gident);
		man_var_tab_declare_env(true, false, true);

	}else{
		if(dtoken == DT_LIB_FUNC)
			fprintf(CTT_BUF, "function _.%s", gident);
		else
			fprintf(CTT_BUF, "local function %s", gident);
	}

	dtoken = DT_NULL;
}

static void search_func_param_end(void){
	for(short i = 0; gident[i] != '\0'; i++){
		if(gident[i] == ')'){
			functd.parameter_end = true;
			break;
		}
	}
}
