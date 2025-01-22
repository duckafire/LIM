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
static char *gident, *gtable_key; // Global

#define IS_ROOT (layer == 0)
#define CUR_CTT_BUF ((IS_ROOT) ? lim.buffers.destine_file : lim.buffers.local.top->content)
#define IFORMAT(t) ((t == NULL) ? "%s" : "%s%s")


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

	if(denv_top->local.prefix){
		gident = tmp;
		gtable_key = NULL;
		build_vt_declaration(false);
	}

	else if(is_local)
		new_treat_env(false);
}

void treat_ident(char *ident, char *table_key){
	if(denv_top->local.prefix){
		gident = ident;
		gtable_key = table_key;
		build_vt_declaration(true);
		return;
	}

	// use or call
	ident_nick = get_nickname_of(ident, IS_ROOT);
	fprintf(CUR_CTT_BUF, IFORMAT(table_key), ident_nick, table_key);
}


static bool is_special_cvalue(void){
	char *tmp;
	tmp = strchr("+-*/^#=:.;()[]{}\"'", gident[0]);

	return (tmp != NULL);
}

static void merge_cur_str_with_bvtail(bool special_cvalue, bool expect_comma){
	denv_top->local.special_cvalue = special_cvalue;
	denv_top->local.expect_comma   = expect_comma;

	char *stmp, **ident;

	ident = &(denv_top->local.bvtail->ident);
	stmp = malloc(strlen(*ident) + strlen(gident) + sizeof(char) + ((gtable_key == NULL) ? 0 : strlen(gtable_key)));
	strcpy(stmp, *ident);
	if(gtable_key == NULL)
		strcat(stmp, gtable_key);
	strcat(stmp, gident);

	free(*ident);
	*ident = stmp;
}

static void vt_comma_or_cvalue(void){
	if(denv_top->local.special_cvalue && !is_special_cvalue()){
		merge_cur_str_with_bvtail(false, true);
		return;
	}

	if(gident[0] == ','){
		denv_top->local.expect_comma = false;
		return;
	}

	if(is_special_cvalue()){
		merge_cur_str_with_bvtail(true, false);
		return;
	}

	put_vt_declaration();
}

static void build_vt_declaration(bool expect_ident){
	if(expect_ident && !denv_top->local.sign_found){
		if(!denv_top->local.expect_comma){
			up_vt_declaration(false);
			denv_top->local.expect_comma = true;

			return;
		}

		put_vt_declaration();
		return;
	}
	
	// local ident0{,} ident1 {= }
	if(!denv_top->local.sign_found){
		
		if(gident[0] == '='){
			denv_top->local.sign_found = true;
			denv_top->local.expect_comma = false;

		}else if(denv_top->local.expect_comma || denv_top->local.special_cvalue){
			vt_comma_or_cvalue();

		}else{
			up_vt_declaration(false);
		}
		
		return;
	}

	if(denv_top->local.expect_comma || denv_top->local.special_cvalue){
		vt_comma_or_cvalue();
		return;
	}

	if(isalnum(gident[0]) || gident[0] == '_' || gident[0] == '"' || gident[0] == '\'' || gident[0] == '{'){
		denv_top->local.expect_comma = true;
		up_vt_declaration( (isalpha(gident[0]) && is_from_lua(gident, CKIA_LUA_KW)) );

		return;
	}

	put_vt_declaration();
}

static void up_vt_declaration(bool is_const){
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
		*buf = qee_create(gident, gtable_key, NULL, is_const);
		(*qtt)++;

	}else{
		if(qee_add_item(buf, gident, gtable_key, NULL, is_const, false))
			(*qtt)++;
	}

	Queue *p;
	for(p = *buf; p->next != NULL; p = p->next);
	denv_top->local.bvtail = p;


	qee_bigger_to_lower(true);
}

static void put_vt_declaration(void){
	if(!denv_top->local.sign_found){
		drop_treat_env();
		return;
	}

	Queue *buf, *cur;
	unsigned int i;
	const unsigned int min_qtt = ((denv_top->local.qident < denv_top->local.qvalue) ? denv_top->local.qident : denv_top->local.qvalue);
	bool is_bident;
	Queue **bdest;
	char format[5];

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

				if(!is_special_cvalue())
					fputc(',', CUR_CTT_BUF);
			}

			if(buf == denv_top->local.bident || (buf != denv_top->local.bident && !cur->is_const)){
				if(is_bident){
					bdest = (IS_ROOT) ? &(lim.buffers.root.global_var_tab) : &(lim.buffers.local.top->local_var_tab);

					ident_nick = save_ident_in_buffer(cur->ident, NULL, IS_ROOT, SCOPE_IDENT, bdest);
				}else{
					ident_nick = get_nickname_of(cur->ident, IS_ROOT);
				}

				fprintf(CUR_CTT_BUF, IFORMAT(gtable_key), ident_nick, cur->table_key);
				continue;
			}

			fprintf(CUR_CTT_BUF, IFORMAT(gtable_key), ident_nick, cur->table_key);
		}

		if(buf == denv_top->local.bvalue)
			break;
	}

	drop_treat_env();
}
