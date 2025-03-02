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
#include "layer-env.h"
#include "nick-man.h"

static char *gident_nick, *gident, *gtable_key; // Global
static DECLARE_TOKEN dtoken = DT_NULL;
static bool space_is_mandatory = false;

struct { bool start_declare; bool expect_comma; }       locald = {false, false};
struct { bool start_declare; bool parameter_end; }      functd = {false, false};
struct { bool start_declare, expect_comma, kw_placed; } floopd = {false, false};

#define IS_FPARAM  (functd.start_declare && !functd.parameter_end)
#define FORMAT(tk) ((tk == NULL) ? "%s" : "%s%s")
#define MIN(a, b)  ((a < b) ? a : b)

#define BUF_LIB_FUNC &(lim.env_buf.lib_func)
#define BUF_VAR      ((lim.env_buf.lenv_quant == 0) ? &(lim.env_buf.var)       : &(lim.env_buf.lenv_stack_top->var))
#define BUF_FUNC     ((lim.env_buf.lenv_quant == 0) ? &(lim.env_buf.func)      : &(lim.env_buf.lenv_stack_top->func))
#define BUF_SPECIAL  &(lim.env_buf.lenv_stack_top->special)
#define BUF_CONTENT  ((lim.env_buf.lenv_quant == 0) ? lim.env_buf.destine_file : lim.env_buf.lenv_stack_top->content)

void treat_const(char *str){
	const bool is_local = (strcmp(str, "local")    == 0);
	const bool is_funct = (strcmp(str, "function") == 0);
	const bool is_floop = (strcmp(str, "for")      == 0);

	gident     = str;
	gtable_key = NULL;


	// checking included
	add_layer_env(gident, NULL);
	pop_layer_env(gident, false);


	// parameters
	if(IS_FPARAM){
		fprintf(BUF_CONTENT, "%s", gident);
		search_fparam_end();

		if(functd.parameter_end)
			space_is_mandatory = false;

	// declaration "bases"
	}else if(dtoken == DT_LIB_FUNC && gident[0] == '('){
		declare_function(true);
		add_layer_env(NULL, NULL);
		search_fparam_end();

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
	}else if(floopd.start_declare){
		default_const_treatment(str);

		if(floopd.expect_comma && str[0] == ',')
			floopd.expect_comma = false;
		else
			floopd.start_declare = false;

	}else if(is_floop){
		floopd.start_declare = true;
		floopd.expect_comma  = false;
		floopd.kw_placed     = false;
	
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
		check_mandatory_space("f");
		declare_function(false);
		add_layer_env(NULL, gtable_key);
		return;

	}else if(dtoken == DT_LOCAL){
		locald.start_declare = true;
		locald.expect_comma  = true;
		default_const_treatment("local "); // space included
	}

	if(IS_FPARAM){
		gident_nick = save_ident(gident, NULL, NICK_PARAM, BUF_SPECIAL);
		fprintf(BUF_CONTENT, FORMAT(gtable_key), gident_nick, gtable_key);
		return;
	}

	if(floopd.start_declare){
		if(!floopd.kw_placed){
			floopd.kw_placed = true;
			default_const_treatment("for "); // space included
		}

		if(!floopd.expect_comma){
			fprintf(BUF_CONTENT, "%s", save_ident(gident, NULL, NICK_FOR_LOOP, BUF_SPECIAL));
			space_is_mandatory = true;
			floopd.expect_comma = true;
			return;
		}

		floopd.start_declare = false;
	}

	// use or call
	check_mandatory_space(gident);

	if(strcmp(gident, "_G") == 0){
		fprintf(BUF_CONTENT, "_.%s", &(gtable_key[1]) );
		return;
	}

	if(locald.start_declare)
		gident_nick = save_ident(gident, gtable_key, NICK_IDENT, BUF_VAR);
	else
		gident_nick = get_nickname_of(gident, false);

	fprintf(BUF_CONTENT, FORMAT(gtable_key), gident_nick, gtable_key);
}

void treat_standard(char *_ident, char *_table_key, Queue **buf){
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


	nick = get_nickname_of(full, true);
	if(strcmp(nick, full) == 0) // nickname not defined
		nick = save_ident(full, NULL, NICK_STD_HDR, buf);


	check_mandatory_space(nick);

	fprintf(BUF_CONTENT, "%s", nick);

	if(allocad)
		free(full);

	dtoken = DT_NULL;
	locald.start_declare = locald.expect_comma  = false;
	functd.start_declare = functd.parameter_end = false;
	floopd.start_declare = floopd.expect_comma  = floopd.kw_placed = false;
}

static void default_const_treatment(char *str){
	check_mandatory_space(str);

	dtoken = DT_NULL;
	fprintf(BUF_CONTENT, "%s", str);
}

static void declare_function(bool is_anony){
	if(is_anony){
		if(space_is_mandatory) // gident == "(*"
			fprintf(BUF_CONTENT, " function%s", gident);
		else
			fprintf(BUF_CONTENT, "function%s", gident);

	}else{
		if(dtoken == DT_LIB_FUNC){
			if(gtable_key == NULL)
				fprintf(BUF_CONTENT, "function %s", save_ident(gident, NULL, NICK_LIB_FUNC, BUF_LIB_FUNC));
			else
				fprintf(BUF_CONTENT, "function %s%s", get_nickname_of(gident, false), gtable_key);

		}else{
			fprintf(BUF_CONTENT, "local function %s", save_ident(gident, NULL, NICK_IDENT, BUF_FUNC));

			if(gtable_key != NULL)
				pverbose(V_WARNING, "An invalid table key was discarted:", "local function", gident, ">", gtable_key, "<", NULL);
		}
	}

	functd.start_declare = true;
	functd.parameter_end = false;
	dtoken = DT_NULL;
}

static void search_fparam_end(void){
	for(short i = 0; gident[i] != '\0'; i++){
		if(gident[i] == ')'){
			functd.parameter_end = true;
			break;
		}
	}
}

static void check_mandatory_space(char *str){
	char firstc = str[0];
	char lastc = str[ strlen(str) - 1 ];

	// check now
	if(space_is_mandatory && (firstc == '_' || isalnum(firstc)))
		fprintf(BUF_CONTENT, " ");

	// set to future checking
	space_is_mandatory = (lastc == '_' || isalnum(lastc));
}
