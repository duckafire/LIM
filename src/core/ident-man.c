#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "tools/lim-global-variables.h"
#include "tools/queue.h"
#include "tools/string-plus.h"
#include "ident-man.h"

// #0: nickname to lua standard and header identifiers;
// NICK_CURRENT, NICK_FORMAT, NICK_FIRST, NICK_LAST, NICK_PREFIX, NICK_SAVEDS
static char *nick_std_hdr[5]      = {NULL, NULL, "A", "Z", "\0"};
static char *nick_global_ident[5] = {NULL, NULL, "a", "z", "G",};
static char *nick_local_ident[6]  = {NULL, NULL, "a", "z", "L",  NULL}; // nickname
static char *nick_parameter[6]    = {NULL, NULL, "a", "z", "\0", NULL}; // ~

static const short LEN_2C = sizeof(char) * 2;

#define NICK_CURRENT(n) (n[0])
#define NICK_FORMAT(n)  (n[1])
#define NICK_FIRSTC(n)  (n[2][0])
#define NICK_FIRST(n)   (n[2])
#define NICK_LASTC(n)   (n[3][0])
#define NICK_LAST(n)    (n[3])
#define NICK_PREFIX(n)  (n[4])
#define NICK_SAVEDS(n)  (n[5])

void start_nickname_buffers(void){
	start_nick_buf(nick_std_hdr);
	start_nick_buf(nick_global_ident);
	start_nick_buf(nick_local_ident);
	start_nick_buf(nick_parameter);
}

void restart_local_parameter_nicknames(void){
	restart_nickname_of(nick_local_ident);
	restart_nickname_of(nick_parameter);
}

static void restart_nickname_of(char *nick_buf[]){
	free_nick_buf(nick_buf, false);
	start_nick_buf(nick_buf);
	NICK_CURRENT(nick_buf) = NICK_SAVEDS(nick_buf);
	NICK_SAVEDS(nick_buf) = NULL;
}

void save_local_parameter_state(void){
	save_state_of(nick_local_ident);
	save_state_of(nick_parameter);
}

static void save_state_of(char *nick_buf[]){
	NICK_SAVEDS(nick_buf) = malloc( strlen( NICK_CURRENT(nick_buf) + 1 ) );
	strcpy(NICK_SAVEDS(nick_buf), NICK_CURRENT(nick_buf));
}

static void start_nick_buf(char *nick_buf[]){
	NICK_CURRENT( nick_buf ) = malloc(LEN_2C);
	strcpy(NICK_CURRENT( nick_buf ), NICK_FIRST( nick_buf ));
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

void free_nickname_buffers(bool saveds_included){
	free_nick_buf(nick_std_hdr,      saveds_included);
	free_nick_buf(nick_global_ident, saveds_included);
	free_nick_buf(nick_local_ident,  saveds_included);
	free_nick_buf(nick_parameter,    saveds_included);
}

static void free_nick_buf(char *nick_buf[], bool saveds_included){
	free( NICK_CURRENT(nick_buf) );
	free( NICK_FORMAT(nick_buf) );
	nick_buf[0] = NULL; // CURRENT
	nick_buf[1] = NULL; // FORMAT

	// call only in end of "read_source_file"
	if(saveds_included)
		free( NICK_SAVEDS(nick_buf) );
}


void new_local_environment(void){
	Func_Env_Stack *new;

	new = malloc(sizeof(Func_Env_Stack));
	new->content       = tmpfile();
	new->scope_var_tab = NULL;
	new->local_func    = NULL;
	new->local_var_tab = NULL;
	new->parameter     = NULL;
	new->below         = NULL;

	if(lim.buffers.local.bottom == NULL){
		lim.buffers.local.bottom = new;
		lim.buffers.local.top = new;
		return;
	}

	new->below = lim.buffers.local.top;
	lim.buffers.local.top = new;
}

void drop_local_environment(char **anony_func_to_local_declare){
	char c = 0, d = 0;
	Func_Env_Stack *top;

	top = lim.buffers.local.top;
	lim.buffers.local.top = top->below;

	if(lim.buffers.local.top == NULL)
		lim.buffers.local.bottom = NULL;

	fseek(top->content, 0, SEEK_SET);
	if(top->scope_var_tab != NULL)
		fseek(top->scope_var_tab, 0, SEEK_SET);

	#define PUT(c) ((anony_func_to_local_declare != NULL) ? string_add(anony_func_to_local_declare, c) : fputc(c, ((lim.buffers.local.top == NULL) ? lim.buffers.destine_file : lim.buffers.local.top->content)))

	if(anony_func_to_local_declare != NULL)
		string_set(anony_func_to_local_declare, STR_START);

	while( (c = fgetc(top->content)) != EOF ){
		PUT(c);

		if(top->scope_var_tab != NULL && d != EOF && c == ')'){
			while( (d = fgetc(top->scope_var_tab)) != EOF)
				PUT(d);

			PUT(';');
		}
	}

	#undef PUT

	fclose(top->content);
	if(top->scope_var_tab != NULL)
		fclose(top->scope_var_tab);
	
	qee_free_queue(top->local_func);
	qee_free_queue(top->local_var_tab);
	qee_free_queue(top->parameter);

	free(top);
}

char* save_ident_in_buffer(char *ident, char *table_key, bool is_root, SCOPE_ID id, Queue **buf){
	// get a new nickname to "ident"
	char *nick_tmp, **nick_buf;

	if(ident[0] != '_'){

		if(id == SCOPE_IDENT){
			if(is_root)
				nick_buf = nick_global_ident;
			else
				nick_buf = nick_local_ident;

		}else if(id == SCOPE_STD_HDR)
			nick_buf = nick_std_hdr;

		else if(id == SCOPE_PARAM)
			nick_buf = nick_parameter;

		nick_tmp = get_and_update_nick(nick_buf);

	}else{
		// identifier that start with
		// '_' will not be compact
		nick_tmp = ident;
	}


	// write identifier nickname to scope buffer
	if(id == SCOPE_IDENT){ // var/tab/func
		FILE **scope_var_tab;

		if(is_root)
			scope_var_tab = &(lim.buffers.root.scope_var_tab);
		else
			scope_var_tab = &(lim.buffers.local.top->scope_var_tab);

		if(*scope_var_tab == NULL){
			*scope_var_tab = tmpfile();

			fprintf(*scope_var_tab , "local ");
		}else{
			fputc(',', *scope_var_tab);
		}

		fprintf(*scope_var_tab , "%s", nick_tmp);

	}else if(id == SCOPE_STD_HDR){
		FILE **pointer, **address;
		pointer = &(lim.buffers.root.scope_func_pointer);
		address = &(lim.buffers.root.scope_func_address);

		if(*pointer == NULL){
			*pointer = tmpfile();
			*address = tmpfile();

			fprintf(*pointer, "local ");
			fputc('=', *address);
		}else{
			fputc(',', *pointer);
			fputc(',', *address);
		}

		fprintf(*pointer, "%s", nick_tmp);
		fprintf(*address, "%s", ident);
	}


	if(*buf == NULL)
		*buf = qee_create(ident, table_key, nick_tmp, false);
	else
		qee_add_item(buf, ident, table_key, nick_tmp, false, QEE_DROP);

	return nick_tmp;
}

char* get_nickname_of(char *ident, bool is_root){
	Queue *cur, *bufs[3];

	if(ident[0] == '_')
		return ident;

	if(!is_root){
		Func_Env_Stack *env;

		for(env = lim.buffers.local.top; env != NULL; env = env->below){
			bufs[0] = env->local_func;
			bufs[1] = env->local_var_tab;
			bufs[2] = env->parameter;

			for(short i = 0; i < 3; i++){
				cur = qee_get_item(bufs[i], ident);

				if(cur != NULL)
					return cur->nick;
			}
		}
	}


	bufs[0] = lim.buffers.root.global_func;
	bufs[1] = lim.buffers.root.global_var_tab;

	for(short i = 0; i < 2; i++){
		cur = qee_get_item(bufs[i], ident);

		if(cur != NULL)
			return cur->nick;
	}


	// here should have an error ;)
	return ident;
}
