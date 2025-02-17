#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <ctype.h>
#include "tools/queue.h"
#include "tools/lim-global-variables.h"
#include "tools/string-plus.h"
#include "tools/verbose.h"
#include "check-content.h"
#include "treat.h"
#include "ident-man.h"

static char *gident_nick, *gident, *gtable_key; // Global
static const char* lua_operator_kw[3] = {"and","not","or"};
static const char* lua_boolean_kw[3] = {"false","nil","true"};
static char *anony_func_to_local_declare = NULL;
static DECLARE_TOKEN dtoken = DT_NULL;
static Stack_Env *locald = NULL; // LOCAL Declaration
static bool space_is_mandatory = false;
struct{ bool start_declare; bool parameter_end; }functd = {false, false};
struct { unsigned short height; Layer_Type *type; }layer = {0, NULL};
struct { bool start_declare, expect_comma, kw_placed; } for_loop = {false, false};

#define IS_ROOT      (layer.height == 0)
#define BUF_FUNC     ((IS_ROOT) ? &(lim.buffers.root.global_func)    : &(lim.buffers.local.top->local_func))
#define BUF_VAR_TAB  ((IS_ROOT) ? &(lim.buffers.root.global_var_tab) : &(lim.buffers.local.top->local_var_tab))
#define BUF_FOR_LOOP ((IS_ROOT) ? &(lim.buffers.root.global_for_loop): &(lim.buffers.local.top->local_for_loop))
#define CTT_BUF      ((IS_ROOT) ? lim.buffers.destine_file : lim.buffers.local.top->content)
#define FORMAT(tk)  ((tk == NULL) ? "%s" : "%s%s")
#define MIN(a, b)   ((a < b) ? a : b)
#define IS_LUA_KW   (isalpha(gident[0]) && is_from_lua(gident, CKIA_LUA_KW))
#define LOCALD_ON   (locald != NULL && locald->start_declare)

void finish_treatment(void){
	gident = gtable_key = NULL;

	while(locald != NULL){
		print_local_declare(PLD_FORCED_END);
		drop_var_tab_declare_env(true);
	}
}

void treat_const(char *str){
	const bool is_local = (strcmp(str, "local")    == 0);
	const bool is_funct = (strcmp(str, "function") == 0);
	const bool is_floop = (strcmp(str, "for")      == 0);
	const bool is_endkw = (strcmp(str, "end")      == 0);
	const bool is_func_param_declare = (functd.start_declare && !functd.parameter_end);

	gident     = str;
	gtable_key = NULL;

	if(!is_func_param_declare && layer.height > 0){
		if(strcmp(str, "if") == 0 || strcmp(str, "do") == 0)
			update_layer(false);

		else if(is_endkw && downdate_layer())
			if(pop_function_declaration())
				return;

	}else if(is_endkw){
		pop_nicknames_env_to_for_loop(layer.height);
	}

	if(is_func_param_declare){
		fprintf(CTT_BUF, "%s", gident);
		search_func_param_end();

	}else if(dtoken == DT_LIB_FUNC && gident[0] == '('){
		start_function_declaration(true);
		search_func_param_end();

	}else if(dtoken == DT_LOCAL && is_funct){
		dtoken = DT_FUNCTION;

	}else if(LOCALD_ON){
		if(!locald->attrib_start)
			treat_local_declare_BEFORE_comma(NULL);
		else
			treat_local_declare_AFTER_comma(false);

	}else if(is_funct){
		dtoken = DT_LIB_FUNC;

	}else if(is_local){
		dtoken = DT_LOCAL;

	}else if(for_loop.start_declare){
		default_const_treatment(str);

		if(for_loop.expect_comma && str[0] == ',')
			for_loop.expect_comma = false;
		else
			for_loop.start_declare = false;

	}else if(is_floop){
		new_nicknames_env_to_for_loop(layer.height);
		for_loop.start_declare = true;
		for_loop.expect_comma  = false;
		for_loop.kw_placed     = false;
	}else{
		default_const_treatment(str);
	}
}

void treat_ident(char *_ident, char *_table_key){
	gident     = _ident;
	gtable_key = _table_key;

	if(dtoken == DT_LOCAL){
		pverbose(V_NEW_THING, count_envs(true) + 1, "Identifier declaration chain");

		if(locald != NULL && !locald->start_declare)
			locald->start_declare = true;
		else
			new_var_tab_declare_env(true, false);

		dtoken = DT_NULL;

	}else if(dtoken == DT_FUNCTION || dtoken == DT_LIB_FUNC){
		check_if_space_is_need("f");
		start_function_declaration(false);
		return;
	}

	if(functd.start_declare && !functd.parameter_end){
		gident_nick = save_ident_in_buffer(gident, NULL, IS_ROOT, SCOPE_PARAM, &(lim.buffers.local.top->parameter));
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
	}else if(for_loop.start_declare){
		if(!for_loop.kw_placed){
			for_loop.kw_placed = true;
			default_const_treatment("for ");
		}

		if(!for_loop.expect_comma){
			fprintf(CTT_BUF, "%s", save_ident_in_buffer(gident, NULL, IS_ROOT, SCOPE_FOR_LOOP, BUF_FOR_LOOP));
			space_is_mandatory = true;
			for_loop.expect_comma = true;
			return;
		}

		for_loop.start_declare = false;
	}

	// use or call
	check_if_space_is_need(gident);
	set_if_space_is_mandatory(gident);

	if(strcmp(gident, "_G") == 0){
		fprintf(CTT_BUF, "_.%s", &(gtable_key[1]) );
		return;
	}

	gident_nick = get_nickname_of(gident, IS_ROOT);
	fprintf(CTT_BUF, FORMAT(gtable_key), gident_nick, gtable_key);
}

void treat_standard_from(bool lua, char *_ident, char *_table_key, Queue **buf){
	bool allocad = true, sended = false;
	char *full, *nick;


	if(_table_key == NULL){
		full = _ident;
		allocad = false;

	}else{
		full = malloc( strlen(_ident) + strlen(_table_key) + sizeof(char) );
		strcpy(full, _ident);
		strcat(full, _table_key);
	}

	nick = get_nickname_of(full, IS_ROOT);
	if(strcmp(nick, full) == 0)
		nick = save_ident_in_buffer(full, NULL, IS_ROOT, SCOPE_STD_HDR, buf);


	gident     = full;
	gtable_key = NULL;

	if(LOCALD_ON){
		if(!locald->attrib_start){
			print_local_declare(PLD_FAIL_IDENT);
		}else{
			treat_local_declare_AFTER_comma(true);
			sended = true;
		}
	}

	if(!sended){
		check_if_space_is_need(nick);
		set_if_space_is_mandatory(nick);

		fprintf(CTT_BUF, "%s", nick);
	}

	if(allocad)
		free(full);
}

static void default_const_treatment(char *str){
	check_if_space_is_need(str);
	set_if_space_is_mandatory(str);

	dtoken = DT_NULL;
	fprintf(CTT_BUF, "%s", str);
}

static unsigned short count_envs(bool is_locald){
	unsigned short qtt = 0;

	if(is_locald){
		Stack_Env *cur;

		for(cur = locald; cur != NULL; cur = cur->below)
			qtt++;
	}else{
		Func_Env_Stack *cur;

		for(cur = lim.buffers.local.top; cur != NULL; cur = cur->below)
			qtt++;
	}

	return qtt;
}


static void update_layer(bool is_func){
	pverbose(V_NEW_THING, ++layer.height, "Layer block");

	Layer_Type *new;

	new = malloc(sizeof(Layer_Type));
	new->is_func_layer = is_func;

	if(layer.type == NULL){
		new->below = NULL;
		layer.type = new;
		return;
	}

	new->below = layer.type;
	layer.type = new;
}

static bool downdate_layer(void){
	pverbose(V_END_THING, layer.height--, "Layer block");

	Layer_Type *tmp;
	tmp = layer.type;
	layer.type = layer.type->below;

	const bool is_func = tmp->is_func_layer;
	free(tmp);

	pop_nicknames_env_to_for_loop(layer.height);
	return is_func;
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
		else if(first != ',' || new_token_id == LT_PAREN_COMMA)
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
		if(!compare_token(LT_USEORCALL, NULL, LT_COMMA, LT_MOPERATOR, LT_LOPERATOR, LT_PARENO, LT_BRACKETO, LT_PAREN_COMMA, LT_NULL)){
			print_local_declare(PLD_FAIL_IDENT);
			return;
		}

		if(*token == LT_COMMA){
			gident = get_nickname_of(gident, IS_ROOT);
			update_local_declare( ((gident[0] == '_') ? PLD_FAIL_CONST : PLD_FAIL_IDENT) );
		}else{
			merge_compound_value( get_nickname_of(gident, IS_ROOT) );
		}

		*token = LT_USEORCALL;
		return;
	}

	if(isalpha(gident[0])){
		short i;

		// boolean
		for(i = 0; i < 3; i++){
			if(strcmp(gident, lua_boolean_kw[i]) == 0){

				if(!compare_token(LT_BOOLEAN, NULL, LT_COMMA, LT_PARENO, LT_LOPERATOR, LT_BRACKETO, LT_PAREN_COMMA, LT_NULL)){
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
			if(*token != LT_COMMA && *token != LT_PAREN_COMMA){
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
		if(!compare_token(LT_NUMBER, NULL, LT_COMMA, LT_BRACKETO, LT_PARENO, LT_MOPERATOR, LT_LOPERATOR, LT_PAREN_COMMA, LT_NULL)){
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
		new_var_tab_declare_env(false, true);
		space_is_mandatory = false;

		for(short i = 1; gident[i] != '\0'; i++){
			if(gident[i] == ')'){
				functd.parameter_end = true;
				break;
			}
		}

		return;
	}

	if(strchr("()", gident[0]) != NULL){ // no return
		char *c;

		for(c = gident; *c != '\0'; c++){
			if(*c == '(')
				(locald->paren_layer)++;

			else if(locald->paren_layer > 0)
				(locald->paren_layer)--;
		}
	}

	if(locald->paren_layer > 0 && common_token_test(',', NULL, LT_PAREN_COMMA, LT_PAREN_COMMA, 1, LT_BOOLEAN, LT_FUNC_END, LT_NUMBER, LT_PARENC, LT_STRING, LT_TABLE, LT_USEORCALL, LT_NULL)) return;

	if(common_token_test(0,"=><~",  LT_LOPERATOR, LT_LOPERATOR, 0, LT_BOOLEAN, LT_BRACKETC, LT_NUMBER, LT_PARENC, LT_STRING, LT_TABLE, LT_USEORCALL, LT_NULL)) return;
	if(common_token_test(0,"+-*/%^",LT_MOPERATOR, LT_MOPERATOR, 0, LT_BRACKETC, LT_NUMBER, LT_PARENC, LT_USEORCALL, LT_NULL)) return;
	if(common_token_test('.', NULL, LT_NULL,      LT_CONCAT,    0, LT_STRING, LT_USEORCALL, LT_NULL)) return;
	if(common_token_test('{', NULL, LT_NULL,      LT_TABLE,     1, LT_BRACKETO, LT_COMMA, LT_PAREN_COMMA, LT_NULL)) return;
	if(common_token_test(0,   "'\"",LT_STRING,    LT_STRING,    1, LT_BRACKETO, LT_COMMA, LT_CONCAT, LT_PAREN_COMMA, LT_LOPERATOR, LT_NULL)) return;
	if(common_token_test(',', NULL, LT_COMMA,     LT_COMMA,     1, LT_BOOLEAN, LT_FUNC_END, LT_NUMBER, LT_PARENC, LT_STRING, LT_TABLE, LT_USEORCALL, LT_NULL)) return;
	if(common_token_test('(', NULL, LT_NULL,      LT_PARENO,    1, LT_BRACKETC, LT_BRACKETO, LT_COMMA, LT_LOPERATOR, LT_MOPERATOR, LT_NUMBER, LT_PARENC, LT_PARENO, LT_PAREN_COMMA, LT_USEORCALL, LT_NULL)) return;
	if(common_token_test(')', NULL, LT_NULL,      LT_PARENC,    1, LT_BRACKETC, LT_BRACKETO, LT_COMMA, LT_LOPERATOR, LT_MOPERATOR, LT_NUMBER, LT_PARENC, LT_PARENO, LT_USEORCALL, LT_NULL)) return;
	if(common_token_test('[', NULL, LT_NULL,      LT_BRACKETO,  0, LT_USEORCALL, LT_NULL)) return;
	if(common_token_test(']', NULL, LT_BRACKETO,  LT_BRACKETC,  0, LT_BOOLEAN, LT_BRACKETC, LT_NUMBER, LT_PARENC, LT_STRING, LT_TABLE, LT_USEORCALL, LT_NULL)) return;

	if(LOCALD_ON)
		print_local_declare(PLD_FAIL_CONST);
}


static void new_var_tab_declare_env(bool start, bool in_ident_decl){
	Stack_Env *new;
	new = malloc(sizeof(Stack_Env));

	if(locald == NULL){
		locald = new;
		locald->below = NULL;

	}else{
		new->below = locald;
		locald       = new;
	}

	// set new / reset
	locald->start_declare = start;
	locald->in_ident_decl = in_ident_decl;
	locald->spc_is_mandat = space_is_mandatory;
	locald->in_root_envir = IS_ROOT;

	locald->attrib_start = false;
	locald->expect_comma = false;

	locald->token = LT_NULL;

	locald->bident = NULL;
	locald->bvalue = NULL;
	locald->bvtail = NULL;

	locald->qident = 0;
	locald->qvalue = 0;

	locald->paren_layer = 0;
}

static void drop_var_tab_declare_env(bool force){
	if(locald == NULL)
		return;

	if(!force && locald->in_ident_decl){
		locald->start_declare = true;
		
		qee_free_queue(locald->bident);
		qee_free_queue(locald->bvalue);

		locald->bident = NULL;
		locald->bvalue = NULL;
		return;
	}

	Stack_Env *below;
	below = locald->below;

	qee_free_queue(locald->bident);
	qee_free_queue(locald->bvalue);
	
	free(locald);
	locald = below;
}

static void merge_compound_value(char *lident){ // Local
	unsigned short total_len;
	char **ident, **takey, *tmp;

	ident = &(locald->bvtail->ident);
	takey = &(locald->bvtail->table_key);


	total_len = sizeof(char) + strlen(*ident) + strlen(lident);

	if(*takey != NULL)
		total_len += strlen(*takey);

	if(gtable_key != NULL)
		total_len += strlen(gtable_key);

	tmp = malloc(total_len);


	strcpy(tmp, *ident);
	if(*takey != NULL)
		strcat(tmp, *takey);

	strcat(tmp, lident);
	if(gtable_key != NULL)
		strcat(tmp, gtable_key);


	free(*ident);
	*ident = tmp;

	free(*takey);
	*takey = NULL;
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

	
	qee_bigger_to_lower(false);
	
	if(*buf == NULL){
		*buf = qee_create(gident, gtable_key, NULL, is_const);
		(*qtt)++;

		if(locald->attrib_start)
			locald->bvtail = *buf;

	}else{
		if(qee_add_item(buf, gident, gtable_key, NULL, is_const, ((*buf == locald->bvalue) ? QEE_INSERT : QEE_DROP))){
			(*qtt)++;

			if(locald->attrib_start)
				locald->bvtail = locald->bvtail->next;
		}
	}

	qee_bigger_to_lower(true);
}

static void print_local_declare(PLD_ID id){
	if(!locald->attrib_start){
		Queue *cur;

		for(cur = locald->bident; cur != NULL; cur = cur->next)
			save_ident_in_buffer(cur->ident, cur->table_key, locald->in_root_envir, SCOPE_IDENT, BUF_VAR_TAB);

		pverbose(V_LOST_THING, count_envs(true), "Identifier declaration chain");
		drop_var_tab_declare_env(false);

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

	FILE *var_tab_ctt_buf;
	var_tab_ctt_buf = ((locald->in_root_envir) ? lim.buffers.destine_file : lim.buffers.local.top->content);

	if(low_length > 0){
		if(locald->spc_is_mandat)
			fprintf(var_tab_ctt_buf, " ");

		for(cur_buf = locald->bident; true; cur_buf = locald->bvalue){
			cur_item = NULL;
			is_ident_buf = (cur_buf == locald->bident);

			for(i = 0; i < low_length; i++){
				if(cur_item == NULL){
					cur_item = cur_buf;

					if(!is_ident_buf)
						fputc('=', var_tab_ctt_buf);
				}else{
					cur_item = cur_item->next;

					fputc(',', var_tab_ctt_buf);
				}

				if(is_ident_buf)
					gident_nick = save_ident_in_buffer(cur_item->ident, cur_item->table_key, locald->in_root_envir, SCOPE_IDENT, BUF_VAR_TAB);
				else
					gident_nick = cur_item->ident;

				fprintf(var_tab_ctt_buf, FORMAT(cur_item->table_key), gident_nick, cur_item->table_key);
			}

			if(is_ident_buf && cur_item != NULL)
				while( (cur_item = cur_item->next) != NULL )
					save_ident_in_buffer(cur_item->ident, cur_item->table_key, locald->in_root_envir, SCOPE_IDENT, BUF_VAR_TAB);

			if(cur_buf == locald->bvalue){
				set_if_space_is_mandatory( ((cur_item->is_const) ? cur_item->ident : cur_item->nick) );
				break;
			}
		}
	}

	pverbose(V_END_THING, count_envs(true), "Identifier declaration chain");
	drop_var_tab_declare_env(false);

	// recycle
	if(id == PLD_FAIL_CONST)
		treat_const(gident);
	else if(id == PLD_FAIL_IDENT)
		treat_ident(gident, gtable_key);
}


static void start_function_declaration(bool is_anony){
	update_layer(true);

	functd.start_declare = true;
	functd.parameter_end = false;
	new_local_environment( (!is_anony && gtable_key != NULL && gtable_key[0] == ':') );

	save_local_parameter_state();

	if(is_anony){
		if(space_is_mandatory && (locald == NULL || locald->token != LT_FUNC_START))
			fprintf(CTT_BUF, " function%s", gident);
		else
			fprintf(CTT_BUF, "function%s", gident);

	}else{
		if(dtoken == DT_LIB_FUNC){
			if(gtable_key == NULL)
				fprintf(CTT_BUF, "function %s", save_lib_func_in_buffer(gident));
			else
				fprintf(CTT_BUF, "function %s%s", get_nickname_of(gident, IS_ROOT), gtable_key);

		}else{
			// NOTE: like way of avoid that the local
			// nickname prefix will be putted in this
			// function nickname, it don't use 'IS_ROOT'.
			fprintf(CTT_BUF, ((gtable_key == NULL) ? "%s=function" : "%s%s=function"), save_ident_in_buffer(gident, gtable_key, (layer.height - 1 == 0), SCOPE_IDENT, BUF_FUNC), gtable_key);
		}
	}

	dtoken = DT_NULL;
	pverbose(V_NEW_THING, count_envs(false), "Function declaration");
}

static void search_func_param_end(void){
	for(short i = 0; gident[i] != '\0'; i++){
		if(gident[i] == ')'){
			functd.parameter_end = true;
			break;
		}
	}
}

static bool pop_function_declaration(void){
	const bool in_ident_declare = (locald == NULL) ? false : locald->in_ident_decl;

	if(locald != NULL && !locald->in_root_envir)
		print_local_declare(PLD_FORCED_END);

	if(in_ident_declare){
		drop_var_tab_declare_env(true);

		if(LOCALD_ON)
			locald->token = LT_FUNC_END;
	}

	pverbose(V_END_THING, count_envs(false), "Function declaration");
	restart_local_parameter_nicknames();
	drop_local_environment( ((locald != NULL && in_ident_declare) ? &anony_func_to_local_declare : NULL) );

	if(anony_func_to_local_declare != NULL){
		gident = anony_func_to_local_declare;
		update_local_declare(true);

		merge_compound_value( ((space_is_mandatory) ? " end" : "end") );
		space_is_mandatory = true;

		string_set(&anony_func_to_local_declare, STR_END);
		return true;
	}
	return false;
}


static void check_if_space_is_need(char *str){
	char firstc = str[0];

	if(space_is_mandatory && (firstc == '_' || isalnum(firstc))){
		fprintf(CTT_BUF, " ");
		space_is_mandatory = false;
	}
}

static void set_if_space_is_mandatory(char *str){
	char lastc = str[ strlen(str) - 1 ];

	space_is_mandatory = (lastc == '_' || isalnum(lastc));
}
