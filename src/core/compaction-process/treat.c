#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "../buf-man/buf-man.h"
#include "../lim-global-variables.h"
#include "../string-plus.h"
#include "treat.h"
#include "ident-man.h"

// NOTE: if any syntax or semantic error occurr,
// Lim will pretend that none is occurr and
// continue normally :D

static char *ident_nick;
static unsigned short layer;

static bool local_prefix = false;
static bool local_attribute = false;
static bool local_expect_comma = false;
static Queue *local_buf_ident = NULL;
static Queue *local_buf_value = NULL;
static unsigned short local_qtt_ident = 0;
static unsigned short local_qtt_value = 0;

static bool func_prefix = false;
static bool func_parameter = false;
static bool func_expect_comma = false;

#define IS_ROOT (layer == 0)
#define CUR_CTT_BUF ((IS_ROOT) ? lim.buffers.destine_file       : lim.buffers.local.top->content)
#define BUF_FUNC    ((IS_ROOT) ? lim.buffers.root.scope_func    : lim.buffers.local.top->local_func)
#define BUF_VAR_TAB ((IS_ROOT) ? lim.buffers.root.scope_var_tab : lim.buffers.local.top->local_var_tab)

void treat_const(char **tmp){
#define END string_set(tmp, STR_END); return
	const bool is_local = strcmp(*tmp, "local") == 0;
	const bool is_funct = strcmp(*tmp, "function") == 0;

	update_layer();

	// function parameter declaration
	if(func_parameter){
		if((*tmp)[0] == ')'){
			func_parameter = false;
			fputc(CUR_CTT_BUF, ')');
		}else if(func_expect_comma){
			func_expect_comma = false;
			fputc(CUR_CTT_BUF, ',');
		}
		END;
	}

	// "variable chain declaration",
	if(local_prefix && !is_funct){
		if(!local_attribute && (*tmp)[0] == '=')
			local_attribute = true;

		else if(local_expect_comma)
			if((*tmp)[0] == ',')
				local_expect_comma = false;
			else
				declare_var_tab();

		else if(local_attribute && !isalnum((*tmp)[0]) && (*tmp)[0] != '_' && (*tmp)[0] != ',') // invalid value
			declare_var_tab();

		END;
	}

	// function or variables declaration
	if(is_funct){
		func_prefix = true;
		func_parameter = true;
		func_expect_comma = true;
		END;

	}else if(is_local){
		local_prefix = true;
		local_qtt_ident = 0;
		local_qtt_value = 0;
		local_expect_comma = true;
		END;
	}

	END;
#undef END
}

void treat_ident(char *ident, char *table_key){
	if(func_parameter){
		func_expect_comma = true;
		ident_nick = save_ident_in_buffer(ident, IS_ROOT, SCOPE_PARAM, &(lim.buffers.local.top->parameter));
		fprintf(CUR_CTT_BUF, "%s", ident_nick, NULL);
		return;
	}
	
	if(local_prefix){
		if(func_prefix){
			declare_function(ident);
			return;
		}

		if(!local_expect_comma){
			var_tab_update_declaration(ident);
			local_expect_comma = true;
		}else{
			declare_var_tab();
		}

		return;
	}

	if(func_prefix){
		declare_function(ident);
		return;
	}
	
	if(strcmp(ident, "_G") == 0){
		fprintf(CUR_CTT_BUF, "_.%s", ident);

		if(local_prefix)
			declare_var_tab();

		return;
	}

	// use or call
	ident_nick = get_nickname_of(ident, IS_ROOT);
	fprintf(CUR_BUF, ((table_key == NULL) ? "%s" : "%s%s"), ident_nick, table_key);
}

void treat_std_hdr(char *ident, char *table_key, bool is_func, bool is_from_lua){
	Queue *buf;

	if(is_from_lua){
		if(is_func)
			buf = lim.buffers.root.func_from_lua;
		else
			buf = lim.buffers.root.table_from_lua;
	}else{
		if(is_func)
			buf = lim.buffers.root.func_from_header;
		else
			buf = lim.buffers.root.table_from_header;
	}

	char format[5] = "%s";
	
	if(table_key != NULL)
		strcat(format, "%s");

	fprintf(CUR_BUF, format, ident, table_key);
}

static void update_layer(void){
	if(is_func || (layer > 0 && (strcmp(*tmp, "if") == 0 || strcmp(*tmp, "do") == 0))){
		layer++;

		if(is_func)
			new_local_environment();

		if(local_prefix)
			declare_var_tab();

		if(func_parameter){
			func_parameter = false;
			fputc(CUR_CTT_BUF, ')');
		}

	}else if(layer > 0 && strcmp(*tmp, "end") == 0){
		drop_local_environment();
		layer--;

		if(local_prefix)
			declare_var_tab();

		if(func_parameter){
			func_parameter = false;
			fputc(CUR_CTT_BUF, ')');
		}
	}
}

static void var_tab_update_declaration(char *ident){
	Queue **buf;
	unsigned short *qtt;


	if(!local_attribute){
		buf = &local_buf_ident;
		qtt = &local_qtt_ident;
	}else{
		buf = &local_buf_value;
		qtt = &local_qtt_value;
	}


	if(*buf == NULL){
		*buf = qee_create(NULL, ident);
		(*qtt)++;

	}else{
		if(qee_add_item(buf, NULL, ident, false))
			(*qtt)++;
	}
}

static void declare_var_tab(void){
	if(local_attribute){
		Queue *buf = NULL, *cur = NULL;
		unsigned int i;
		const unsigned int min_qtt = ((local_qtt_ident < local_qtt_value) ? local_qtt_ident : local_qtt_value);

		for(buf = local_buf_ident; true; buf = local_buf_value){
			
			for(i = 0; i < min_qtt; i++){
				if(cur == NULL){
					cur = buf;
					
					if(buf == local_buf_ident)
						fputs(CUR_CTT_BUF, "local ");
					else
						fputc(CUR_CTT_BUF, '=');

				}else{
					cur = cur->next;
					fputc(CUR_CTT_BUF, ',');
				}
				
				ident_nick = save_ident_in_buffer(cur->content[1], IS_ROOT, SCOPE_IDENT, &(BUF_VAR));
				fprintf(CUR_CTT_BUF, "%s", ident_nick);
			}

			if(buf == local_buf_value)
				break;
		}

		return;
	}

	qee_free_queue(local_buf_ident);
	qee_free_queue(local_buf_value);

	local_buf_ident = local_buf_value = NULL;
	local_prefix    = local_attribute = false;
}

static void declare_function(char *ident){
	char format[17] = "local function ";

	if(local_prefix)
		strcat(format, "_.%s");
	else
		strcat(format, "%s");

	ident_nick = save_ident_in_buffer(ident, IS_ROOT, SCOPE_IDENT, BUF_FUNC);
	fprintf(CUR_CTT_BUF, format, ident_nick);
	local_prefix = func_prefix = false;
}
