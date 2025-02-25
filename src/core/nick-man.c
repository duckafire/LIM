#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include "tools/lim-global-variables.h"
#include "tools/queue.h"
#include "tools/string-plus.h"
#include "nick-man.h"

// #0: nickname to lua standard and header identifiers;
// NICK_CURRENT, NICK_FORMAT, NICK_FIRST, NICK_LAST, NICK_PREFIX, NICK_SAVEDS
static char *nick_std_hdr[5]      = {NULL, NULL, "A", "Z", "\0"};
static char *nick_global_ident[5] = {NULL, NULL, "a", "z",  "G"};
static char *nick_local_ident[5]  = {NULL, NULL, "a", "z",  "L"};
static char *nick_parameter[5]    = {NULL, NULL, "a", "z", "\0"};
static char *nick_for_loop[5]     = {NULL, NULL, "a", "z",  "F"};

static const short LEN_2C = sizeof(char) * 2;
static char *nick_lib_func = NULL; // different algorithm

static char *cur_nick, **nick_buf;

#define NICK_CURRENT(n) (n[0])
#define NICK_FORMAT(n)  (n[1])
#define NICK_FIRSTC(n)  (n[2][0])
#define NICK_FIRST(n)   (n[2])
#define NICK_LASTC(n)   (n[3][0])
#define NICK_LAST(n)    (n[3])
#define NICK_PREFIX(n)  (n[4])

void start_nickname_buffers(void){
	start_nick_buf(nick_std_hdr);
	start_nick_buf(nick_global_ident);
	start_nick_buf(nick_local_ident);
	start_nick_buf(nick_parameter);
	start_nick_buf(nick_for_loop);
}

static void start_nick_buf(char *nick_buf[]){
	NICK_CURRENT( nick_buf ) = malloc(LEN_2C);
	strcpy(NICK_CURRENT( nick_buf ), NICK_FIRST( nick_buf ));
}


// use in "./src/core/layer-env.c"
void save_nicknames_state(Local_Env *lenv){
	lenv->save_local_ident = string_copy( NICK_CURRENT(nick_local_ident) );
	lenv->save_parameter   = string_copy( NICK_CURRENT(nick_parameter) );
	lenv->save_for_loop    = string_copy( NICK_CURRENT(nick_for_loop) );
}

void restart_nicknames_state(Local_Env *lenv){
	free( NICK_CURRENT(nick_local_ident) );
	free( NICK_CURRENT(nick_parameter) );
	free( NICK_CURRENT(nick_for_loop) );

	NICK_CURRENT(nick_local_ident) = lenv->save_local_ident;
	NICK_CURRENT(nick_parameter)   = lenv->save_parameter;
	NICK_CURRENT(nick_for_loop)    = lenv->save_for_loop;
}


static char* get_and_update_nick(char *nick_buf[]){
	free( NICK_FORMAT(nick_buf) );
	NICK_FORMAT(nick_buf) = malloc(strlen( NICK_CURRENT(nick_buf) ) + LEN_2C);

	strcpy( NICK_FORMAT(nick_buf), NICK_PREFIX(nick_buf) );
	strcat( NICK_FORMAT(nick_buf), NICK_CURRENT(nick_buf) );

	update_nick_current( nick_buf, strlen( NICK_CURRENT(nick_buf) ) - 1 );
	return NICK_FORMAT( nick_buf );
}

static void update_nick_current(char *nick_buf[], const int last_char){
	if(last_char > -1){
		if(NICK_CURRENT( nick_buf)[last_char] == NICK_LASTC(nick_buf)){
			NICK_CURRENT(nick_buf)[last_char] =  NICK_FIRSTC(nick_buf);
			update_nick_current(nick_buf, last_char - 1);
			return;
		}

		NICK_CURRENT( nick_buf )[last_char]++;
		return;
	}

	char *buf;
	const unsigned short len = strlen(NICK_CURRENT(nick_buf));

	buf = NICK_CURRENT(nick_buf);
	NICK_CURRENT(nick_buf) = malloc(len + 2);

	strcpy(NICK_CURRENT(nick_buf), buf);
	strcat(NICK_CURRENT(nick_buf), NICK_FIRST(nick_buf));
}

char* save_ident(char *ident, char *table_key, NICK_ID id, Queue **buf){
	if(id == NICK_LIB_FUNC){
		free(nick_lib_func);
		nick_lib_func = malloc(strlen(ident) + 3);

		sprintf(nick_lib_func, "_.%s", ident);
		cur_nick = nick_lib_func;

	}else if(ident[0] != '_'){
		switch(id){
			case NICK_STD_HDR:  nick_buf = nick_std_hdr; break;
			case NICK_IDENT:    nick_buf = ((lim.env_buf.lenv_quant == 0) ? nick_global_ident : nick_local_ident); break;
			case NICK_PARAM:    nick_buf = nick_parameter; break;
			case NICK_FOR_LOOP: nick_buf = nick_for_loop; break;
		}

		cur_nick = get_and_update_nick(nick_buf);

	}else{
		if(ident[1] == '\0') // ident == "_"
			return "__";

		return ident; // ident == "_*"
	}


	if(id == NICK_STD_HDR){
		// their chains are merged (ident += table_key)
		// before this function call
		FILE **p, **a;
		p = &(lim.env_buf.scope_fpointer);
		a = &(lim.env_buf.scope_faddress);

		if(*p == NULL){
			*p = tmpfile();
			*a = tmpfile();

			fprintf(*p, "local ");
			fputc('=', *a);
		}else{
			fputc(',', *p);
			fputc(',', *a);
		}

		fprintf(*p, "%s", cur_nick);
		fprintf(*a, "%s", ident);
	}

	
	if(*buf == NULL)
		*buf = qee_create(ident, table_key, cur_nick, false);
	else
		qee_add_item(buf, ident, table_key, cur_nick, false, QEE_DROP);

	return cur_nick;
}


static bool search_in_buffers(char *ident, short max, ...){
	va_list bufs;
	Queue *cur_buf, *cur_item;

	va_start(bufs, max);

	for(short i = 0; i < max; i++){
		cur_buf = va_arg(bufs, Queue*);

		if(cur_buf == NULL)
			continue;

		cur_item = qee_get_item(cur_buf, ident);

		if(cur_item != NULL){
			cur_nick = cur_item->nick;
			va_end(bufs);
			return true;
		}
	}

	va_end(bufs);
	return false;
}

char* get_nickname_of(char *ident, bool is_std_hdr){
	// only from Lua Standard or header.lim
	if(is_std_hdr){
		if(search_in_buffers(ident, 4,
			lim.env_buf.func_from_lua, lim.env_buf.table_from_lua,
			lim.env_buf.func_from_header, lim.env_buf.table_from_header))
			return cur_nick;

		return ident;
	}

	// they never will be compacted
	if(ident[0] == '_'){
		if(ident[1] == '\0') // ident == "_"
			return "__";

		return ident; // ident == "_*"
	}

	// from input file
	if(lim.env_buf.lenv_quant > 0){
		Local_Env *cur_lenv;

		for(cur_lenv = lim.env_buf.lenv_stack_top; cur_lenv != NULL; cur_lenv = cur_lenv->below)
			if(search_in_buffers(ident, 3, cur_lenv->var, cur_lenv->func, cur_lenv->special))
				return cur_nick;
	}

	if(search_in_buffers(ident, 3, lim.env_buf.var, lim.env_buf.func, lim.env_buf.lib_func))
		return cur_nick;

	return ident;
}


void free_nickname_buffers(void){
	free_nick_buf(nick_std_hdr);
	free_nick_buf(nick_global_ident);
	free_nick_buf(nick_local_ident);
	free_nick_buf(nick_parameter);
	free_nick_buf(nick_for_loop);

	free(nick_lib_func);
}

static void free_nick_buf(char *nick_buf[]){
	free( NICK_CURRENT(nick_buf) );
	free( NICK_FORMAT(nick_buf) );
	nick_buf[0] = NULL; // CURRENT
	nick_buf[1] = NULL; // FORMAT
}
