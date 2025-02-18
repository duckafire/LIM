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
static DECLARE_TOKEN dtoken = DT_NULL;
static bool space_is_mandatory = false;

struct { bool start_declare; bool expect_comma; } locald = {false, false};
struct { bool start_declare; bool parameter_end; }functd = {false, false};
struct { unsigned short height; Layer_Type *type; }layer = {0, NULL};
struct { bool start_declare, expect_comma, kw_placed; } for_loop = {false, false};

#define IS_ROOT    (layer.height == 0)
#define IS_FPARAM  (functd.start_declare && !functd.parameter_end)
#define FORMAT(tk) ((tk == NULL) ? "%s" : "%s%s")
#define MIN(a, b)  ((a < b) ? a : b)

#define BUF_FUNC     ((IS_ROOT) ? &(lim.buffers.root.global_func)    : &(lim.buffers.local.top->local_func))
#define BUF_VAR_TAB  ((IS_ROOT) ? &(lim.buffers.root.global_var_tab) : &(lim.buffers.local.top->local_var_tab))
#define BUF_FOR_LOOP ((IS_ROOT) ? &(lim.buffers.root.global_for_loop): &(lim.buffers.local.top->local_for_loop))
#define CTT_BUF      ((IS_ROOT) ? lim.buffers.destine_file : lim.buffers.local.top->content)

void finish_treatment(void){
	while(lim.buffers.local.top != NULL){
		pverbose(V_WARNING, "function end not found.");
		drop_local_environment();
	}
}

void treat_const(char *str){
	const bool is_local = (strcmp(str, "local")    == 0);
	const bool is_funct = (strcmp(str, "function") == 0);
	const bool is_floop = (strcmp(str, "for")      == 0);
	const bool is_endkw = (strcmp(str, "end")      == 0);

	gident     = str;
	gtable_key = NULL;


	if(!IS_FPARAM && layer.height > 0){
		if(strcmp(str, "if") == 0 || strcmp(str, "do") == 0)
			update_layer(false);

		else if(is_endkw && downdate_layer())
			pop_function_declaration();

		default_const_treatment(gident);
		return;
	}
	
	if(is_endkw){
		pop_nicknames_env_to_for_loop(layer.height);

		default_const_treatment("end");
		return;
	}


	// parameters
	if(IS_FPARAM){
		fprintf(CTT_BUF, "%s", gident);
		search_func_param_end();

	// declaration "bases"
	}else if(dtoken == DT_LIB_FUNC && gident[0] == '('){
		start_function_declaration(true);
		search_func_param_end();

	}else if(dtoken == DT_LOCAL && is_funct){
		dtoken = DT_FUNCTION;

	}else if(is_funct){
		dtoken = DT_LIB_FUNC;

	}else if(is_local){
		dtoken = DT_LOCAL;

	}else if(locald.start_declare){
		locald.expect_comma = false;

		if(gident[0] != ',')
			locald.start_declare = false;

		default_const_treatment(str);

	// for loop
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
	
	// default
	}else{
		default_const_treatment(str);
	}
}

void treat_ident(char *_ident, char *_table_key){
	gident     = _ident;
	gtable_key = _table_key;

	if(locald.start_declare){
		if(gtable_key != NULL || locald.expect_comma)
			locald.start_declare = false;
		else
			locald.expect_comma = true;
	}

	if(dtoken == DT_FUNCTION || dtoken == DT_LIB_FUNC){
		check_if_space_is_need("f");
		start_function_declaration(false);
		return;

	}else if(dtoken == DT_LOCAL){
		locald.start_declare = true;
		locald.expect_comma  = true;
		default_const_treatment("local "); // space included
	}

	if(IS_FPARAM){
		gident_nick = save_ident_in_buffer(gident, NULL, IS_ROOT, SCOPE_PARAM, &(lim.buffers.local.top->parameter));
		fprintf(CTT_BUF, FORMAT(gtable_key), gident_nick, gtable_key);
		return;
	}

	if(for_loop.start_declare){
		if(!for_loop.kw_placed){
			for_loop.kw_placed = true;
			default_const_treatment("for "); // space included
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

	if(locald.start_declare)
		gident_nick = save_ident_in_buffer(gident, gtable_key, IS_ROOT, SCOPE_IDENT, BUF_VAR_TAB);
	else
		gident_nick = get_nickname_of(gident, IS_ROOT);

	fprintf(CTT_BUF, FORMAT(gtable_key), gident_nick, gtable_key);
}

void treat_standard_from(bool lua, char *_ident, char *_table_key, Queue **buf){
	bool allocad = true;
	char *full, *nick;


	if(_table_key != NULL){
		full = malloc( strlen(_ident) + strlen(_table_key) + sizeof(char) );
		strcpy(full, _ident);
		strcat(full, _table_key);

	}else{
		full = _ident;
		allocad = false;
	}


	nick = get_nickname_of(full, IS_ROOT);
	if(strcmp(nick, full) == 0) // nickname not defined
		nick = save_ident_in_buffer(full, NULL, IS_ROOT, SCOPE_STD_HDR, buf);


	check_if_space_is_need(nick);
	set_if_space_is_mandatory(nick);

	fprintf(CTT_BUF, "%s", nick);

	if(allocad)
		free(full);
}

static void default_const_treatment(char *str){
	check_if_space_is_need(str);
	set_if_space_is_mandatory(str);

	dtoken = DT_NULL;
	fprintf(CTT_BUF, "%s", str);
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


static void start_function_declaration(bool is_anony){
	update_layer(true);

	functd.start_declare = true;
	functd.parameter_end = false;
	new_local_environment( (!is_anony && gtable_key != NULL && gtable_key[0] == ':') );

	save_local_parameter_state();

	if(is_anony){
		if(space_is_mandatory)
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
	pverbose(V_NEW_THING, get_local_env_quant(), "Function declaration");
}

static void search_func_param_end(void){
	for(short i = 0; gident[i] != '\0'; i++){
		if(gident[i] == ')'){
			functd.parameter_end = true;
			break;
		}
	}
}

static void pop_function_declaration(void){
	pverbose(V_END_THING, get_local_env_quant(), "Function declaration");

	restart_local_parameter_nicknames();
	drop_local_environment();
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
