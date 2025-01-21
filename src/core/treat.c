#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
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

static char *ident_nick;
static unsigned short layer;
static Declaration_Env *denv_bottom = NULL, *denv_top = NULL;

#define IS_ROOT (layer == 0)
#define CUR_CTT_BUF ((IS_ROOT) ? lim.buffers.destine_file : lim.buffers.local.top->content)

#define VT_IDENT  NULL
#define VT_NIDENT "\0"
#define IS_VT_IDENT(is_ident) ((is_ident) ? VT_IDENT : VT_NIDENT)


void start_treatment(void){
	new_treat_env(true);
}

void finish_treatment(void){
	while(denv_top != NULL)
		put_vt_declaration();
}

static void new_treat_env(bool is_bottom){
	Declaration_Env *new;

	new = malloc(sizeof(Declaration_Env));
	new->below = NULL;

	new->local.prefix         = !is_bottom;
	new->local.sign_found     = false;
	new->local.expect_comma   = false;
	new->local.special_cvalue = false;

	new->local.bident = NULL;
	new->local.bvalue = NULL;
	new->local.bvtail = NULL;
	
	new->local.qident = 0;
	new->local.qvalue = 0;

	new->local.ifunct = 0;
	new->local.itable = 0;

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


void treat_const(char *tmp){
	const bool is_local = strcmp(tmp, "local")    == 0;
	const bool is_funct = strcmp(tmp, "function") == 0;

	if(denv_top->local.prefix)
		build_vt_declaration(tmp[0], tmp, false);

	else if(is_local)
		new_treat_env(false);
}

void treat_ident(char *ident, char *table_key){
	if(denv_top->local.prefix){
		build_vt_declaration(ident[0], ident, true);
		return;
	}

	// use or call
	ident_nick = get_nickname_of(ident, IS_ROOT);
	fprintf(CUR_CTT_BUF, ((table_key == NULL) ? "%s" : "%s%s"), ident_nick, table_key);
}


static bool is_special_cvalue(char first){
	char *tmp;
	tmp = strchr("+-*/^#=:.;()[]{}\"'", first);

	return (tmp != NULL);
}

static void merge_cur_str_with_bvtail(char *str, bool special_cvalue, bool expect_comma){
	denv_top->local.special_cvalue = special_cvalue;
	denv_top->local.expect_comma   = expect_comma;

	char *stmp, **cont1;

	cont1 = &(denv_top->local.bvtail->content[1]);
	stmp = malloc(strlen(*cont1) + strlen(str) + sizeof(char));
	strcpy(stmp, *cont1);
	strcat(stmp, str);

	free(*cont1);
	*cont1 = stmp;
}

static void vt_comma_or_cvalue(char c, char *str){
	if(denv_top->local.special_cvalue && !is_special_cvalue(c)){
		merge_cur_str_with_bvtail(str, false, true);
		return;
	}

	if(c == ','){
		denv_top->local.expect_comma = false;
		return;
	}

	if(is_special_cvalue(c)){
		merge_cur_str_with_bvtail(str, true, false);
		return;
	}

	put_vt_declaration();
}

static void build_vt_declaration(char c, char *str, bool expect_ident){
	if(expect_ident && !denv_top->local.sign_found){
		if(!denv_top->local.expect_comma){
			up_vt_declaration(str, false);
			denv_top->local.expect_comma = true;

			return;
		}

		put_vt_declaration();
		return;
	}
	
	// local ident0{,} ident1 {= }
	if(!denv_top->local.sign_found){
		
		if(c == '='){
			denv_top->local.sign_found = true;
			denv_top->local.expect_comma = false;

		}else if(denv_top->local.expect_comma || denv_top->local.special_cvalue){
			vt_comma_or_cvalue(c, str);

		}else{
			up_vt_declaration(str, false);
		}
		
		return;
	}

	if(denv_top->local.expect_comma || denv_top->local.special_cvalue){
		vt_comma_or_cvalue(c, str);
		return;
	}

	if(isalnum(c) || c == '_' || c == '"' || c == '\'' || c == '{'){
		denv_top->local.expect_comma = true;
		up_vt_declaration( str, (isalpha(c) && !is_from_lua(str, CKIA_LUA_KW)) );

		return;
	}

	put_vt_declaration();
}

static void up_vt_declaration(char *ident, bool is_ident){
	Queue **buf;
	unsigned short *qtt;


	if(!denv_top->local.sign_found){
		buf = &denv_top->local.bident;
		qtt = &denv_top->local.qident;
	}else{
		buf = &denv_top->local.bvalue;
		qtt = &denv_top->local.qvalue;
	}

	qee_bigger_to_lower(false);

	if(*buf == NULL){
		*buf = qee_create(IS_VT_IDENT(is_ident), ident);
		(*qtt)++;

	}else{
		if(qee_add_item(buf, IS_VT_IDENT(is_ident), ident, false))
			(*qtt)++;
	}

	Queue *p;
	for(p = *buf; p->next != NULL; p = p->next);
	denv_top->local.bvtail = p;


	qee_bigger_to_lower(true);
}

static void put_vt_declaration(void){
	if(denv_top->local.sign_found){
		Queue *buf, *cur;
		unsigned int i;
		const unsigned int min_qtt = ((denv_top->local.qident < denv_top->local.qvalue) ? denv_top->local.qident : denv_top->local.qvalue);
		bool is_bident;
		Queue **bdest;

		for(buf = denv_top->local.bident; true; buf = denv_top->local.bvalue){
			cur = NULL;
			is_bident = (buf == denv_top->local.bident);
			
			for(i = 0; i < min_qtt; i++){
				if(cur == NULL){
					cur = buf;
					
					if(!is_bident)
						fputc('=', CUR_CTT_BUF);

				}else{
					cur = cur->next;

					if(!is_special_cvalue(cur->content[1][0]))
						fputc(',', CUR_CTT_BUF);
				}
				
				if(buf == denv_top->local.bident || (buf != denv_top->local.bident && string_compare(cur->content[0], VT_IDENT))){

					if(is_bident){
						bdest = (IS_ROOT) ? &(lim.buffers.root.global_var_tab) : &(lim.buffers.local.top->local_var_tab);

						ident_nick = save_ident_in_buffer(cur->content[1], IS_ROOT, SCOPE_IDENT, bdest);
					}else{
						ident_nick = get_nickname_of(cur->content[1], IS_ROOT);
					}

					fprintf(CUR_CTT_BUF, "%s", ident_nick);
					continue;
				}

				fprintf(CUR_CTT_BUF, "%s", cur->content[1]);
			}

			if(buf == denv_top->local.bvalue)
				break;
		}

	}

	drop_treat_env();
}
