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

static bool local_prefix       = false;
static bool local_attribute    = false;
static bool local_expect_comma = false;
static Queue *local_buf_ident  = NULL;
static Queue *local_buf_value  = NULL;
static unsigned short local_qtt_ident = 0;
static unsigned short local_qtt_value = 0;

static unsigned short local_funct_argum   = 0;
static unsigned short local_table_index = 0;

#define IS_ROOT (layer == 0)
#define CUR_CTT_BUF ((IS_ROOT) ? lim.buffers.destine_file : lim.buffers.local.top->content)

void treat_const(char **tmp){
#define END string_set(tmp, STR_END); return
#define IS_C(c) ((*tmp[0]) == c)
	const bool is_local = strcmp(*tmp, "local") == 0;

	// "variable chain declaration",
	if(local_prefix){

		// only ident.; '='; ',' are valid
		if(!local_attribute){
			
			if(IS_C('=')){
				local_attribute = true;
				local_expect_comma = false;

			}else if(local_expect_comma){
				if(IS_C(','))
					local_expect_comma = false;
				else
					declare_var_tab();

			}else{
				// identifier
				var_tab_update_declaration(*tmp);
			}
			
		// all the code is inside these function
		//}else if(var_tab_paren_content(*tmp, local_table_index, '[', ']')){
		//}else if(var_tab_paren_content(*tmp, local_funct_argum, '(', ')')){
		//}else if(var_tab_open_close_paren((*tmp)[0], &local_table_index, '[', ']', "[", "]")){
		//}else if(var_tab_open_close_paren((*tmp)[0], &local_funct_argum, '(', ')', "(", ")")){

		}else if(local_expect_comma){
			if(IS_C(','))
				local_expect_comma = false;
			else
				declare_var_tab();

		// valid values: ident.; '_'; strings; table env;
		// func. arg.; table index;
		}else if(isalnum((*tmp)[0]) || IS_C('_') || IS_C('"') || IS_C('\'') || IS_C('{')){
			local_expect_comma = true;
			var_tab_update_declaration(*tmp);

		}else{
			declare_var_tab();
		}

		END;
	}

	if(is_local){
		local_prefix = true;
		local_expect_comma = false;
		local_qtt_ident = local_qtt_value = local_funct_argum = local_table_index = 0;
		END;
	}

	END;
#undef END
#undef IS_C
}

void treat_ident(char *ident, char *table_key){
	if(local_prefix){
		if(!local_expect_comma){
			var_tab_update_declaration(ident);
			local_expect_comma = true;
		}else{
			declare_var_tab();
		}

		return;
	}

	// use or call
	ident_nick = get_nickname_of(ident, IS_ROOT);
	fprintf(CUR_CTT_BUF, ((table_key == NULL) ? "%s" : "%s%s"), ident_nick, table_key);
}

void treat_end(void){
	declare_var_tab();
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

	qee_bigger_to_lower(false);

	if(*buf == NULL){
		*buf = qee_create(NULL, ident);
		(*qtt)++;

	}else{
		if(qee_add_item(buf, NULL, ident, false))
			(*qtt)++;
	}

	qee_bigger_to_lower(true);
}

static void declare_var_tab(void){
	if(local_attribute){
		Queue *buf, *cur;
		unsigned int i;
		const unsigned int min_qtt = ((local_qtt_ident < local_qtt_value) ? local_qtt_ident : local_qtt_value);

		for(buf = local_buf_ident; true; buf = local_buf_value){
			cur = NULL;
			
			for(i = 0; i < min_qtt; i++){
				if(cur == NULL){
					cur = buf;
					
					if(buf == local_buf_value)
						fputc('=', CUR_CTT_BUF);

				}else{
					cur = cur->next;
					fputc(',', CUR_CTT_BUF);
				}
				
				// TODO: if is it "function"?
				if(buf == local_buf_ident || (buf != local_buf_ident && isalpha(cur->content[1][0]) && strcmp(cur->content[1], "true") != 0 && strcmp(cur->content[1], "false") != 0)){
					if(IS_ROOT)
						ident_nick = save_ident_in_buffer(cur->content[1], true,  SCOPE_IDENT, &(lim.buffers.root.global_var_tab));
					else
						ident_nick = save_ident_in_buffer(cur->content[1], false, SCOPE_IDENT, &(lim.buffers.local.top->local_var_tab));

					fprintf(CUR_CTT_BUF, "%s", ident_nick);
				}else{
					fprintf(CUR_CTT_BUF, "%s", cur->content[1]);
				}
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

static bool var_tab_paren_content(char *ctt, unsigned short qtt, char fc, char ec){
	if(qtt > 0 && ctt[0] != fc && ctt[0] != ec){
		var_tab_update_declaration(ctt);
		return true;
	}

	return false;
}

static bool var_tab_open_close_paren(char fc_tmp, unsigned short *qtt, char fc, char ec, char *fs, char *es){
	const bool is_fc = (fc_tmp == fc);

	if(is_fc || (*qtt > 0 && fc_tmp == ec)){
		if(is_fc){
			local_expect_comma = false;
			(*qtt)++;
		}else{
			(*qtt)--;

			if(*qtt == 0)
				local_expect_comma = true;
		}

		var_tab_update_declaration( ((is_fc) ? fs : es) );
		return true;
	}

	return false;
}
