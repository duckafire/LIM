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

#define IS_ROOT (layer == 0)
#define CUR_CTT_BUF ((IS_ROOT) ? lim.buffers.destine_file : lim.buffers.local.top->content)

void treat_const(char **tmp){
#define END string_set(tmp, STR_END); return
	const bool is_local = strcmp(*tmp, "local") == 0;

	// "variable chain declaration",
	if(local_prefix){

		if(!local_attribute){
			
			if((*tmp)[0] == '='){
				local_attribute = true;
				local_expect_comma = false;

			}else if(local_expect_comma){
				if((*tmp)[0] == ',')
					local_expect_comma = false;
				else
					declare_var_tab();

			}else{
				var_tab_update_declaration(*tmp);
			}

		}else if(local_expect_comma && (*tmp)[0] != ','){
			declare_var_tab();

		}else if(isalnum((*tmp)[0]) || (*tmp)[0] == '_' || (*tmp)[0] == '"' || (*tmp)[0] == '\'' || (*tmp)[0] == '{'){
			var_tab_update_declaration(*tmp);
		}

		END;
	}

	if(is_local){
		local_prefix = true;
		local_qtt_ident = 0;
		local_qtt_value = 0;
		local_expect_comma = false;
		END;
	}

	END;
#undef END
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

void declare_var_tab(void){
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
				
				if(buf == local_buf_ident || (buf != local_buf_ident && isalnum(cur->content[1][0]))){
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
