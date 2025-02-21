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
static char *nick_global_ident[5] = {NULL, NULL, "a", "z",  "G"};
static char *nick_local_ident[5]  = {NULL, NULL, "a", "z",  "L"};
static char *nick_parameter[5]    = {NULL, NULL, "a", "z", "\0"};
static char *nick_for_loop[5]     = {NULL, NULL, "a", "z",  "F"};

static Stack_Nick_Memory *nick_local_mem = NULL;
static Stack_Nick_Memory *nick_param_mem = NULL;

static const short LEN_2C = sizeof(char) * 2;
static char *nick_lib_func = NULL; // different algorithm

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

void restart_local_parameter_nicknames(void){
	restart_nickname_of(nick_local_ident, &nick_local_mem);
	restart_nickname_of(nick_parameter,   &nick_param_mem);
}

void save_local_parameter_state(void){
	save_nickname_of(nick_local_ident, &nick_local_mem);
	save_nickname_of(nick_parameter,   &nick_param_mem);
}

static void restart_nickname_of(char *nick_buf[], Stack_Nick_Memory **mem){
	free_nick_buf(nick_buf);

	if(*mem == NULL){ // for syntax errors in "input code"
		NICK_CURRENT(nick_buf) = string_copy( NICK_FIRST(nick_buf) );
		return;
	}

	NICK_CURRENT(nick_buf) = (*mem)->save;

	if((*mem)->below == NULL){
		free(*mem);
		*mem = NULL;
		return;
	}

	Stack_Nick_Memory *tmp;

	tmp = *mem;
	*mem = tmp->below;

	free(tmp);
}

static void save_nickname_of(char *nick_buf[], Stack_Nick_Memory **mem){
	Stack_Nick_Memory *new;

	new = malloc(sizeof(Stack_Nick_Memory));
	new->save = string_copy( NICK_CURRENT(nick_buf) );
	new->below = NULL;

	if(*mem == NULL){
		*mem = new;
		return;
	}

	new->below = *mem;
	*mem = new;
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

void free_nickname_buffers(void){
	free_nick_buf(nick_std_hdr);
	free_nick_buf(nick_global_ident);
	free_nick_buf(nick_local_ident);
	free_nick_buf(nick_parameter);
	free_nick_buf(nick_for_loop);

	free(nick_lib_func);

	free_nick_mem_stack(nick_local_mem);
	free_nick_mem_stack(nick_param_mem);
}

static void free_nick_buf(char *nick_buf[]){
	free( NICK_CURRENT(nick_buf) );
	free( NICK_FORMAT(nick_buf) );
	nick_buf[0] = NULL; // CURRENT
	nick_buf[1] = NULL; // FORMAT
}

static void free_nick_mem_stack(Stack_Nick_Memory *mem){
	Stack_Nick_Memory *cur, *below;

	for(cur = mem; cur != NULL; cur = below){
		free(cur->save);

		below = cur->below;
		free(cur);
	}
}


void new_nicknames_env_to_for_loop(unsigned short layer_base){
	For_Loop_Stack *new;

	new = malloc( sizeof(For_Loop_Stack) );
	new->save_state = string_copy( NICK_CURRENT(nick_for_loop) );
	new->layer_base = layer_base;
	new->idents     = NULL;
	new->below      = NULL;

	if(lim.buffers.local.stack_top == NULL){
		if(lim.buffers.root.for_loop_stack_top == NULL){
			lim.buffers.root.for_loop_stack_top = new;
			return;
		}

		new->below = lim.buffers.root.for_loop_stack_top;
		lim.buffers.root.for_loop_stack_top = new;
		return;
	}

	if(lim.buffers.local.stack_top->for_loop_stack_top == NULL){
		lim.buffers.local.stack_top->for_loop_stack_top = new;
		return;
	}

	new->below = lim.buffers.local.stack_top->for_loop_stack_top;
	lim.buffers.local.stack_top->for_loop_stack_top = new;
}

void pop_nicknames_env_to_for_loop(unsigned short cur_layer){
	For_Loop_Stack *stack;
	if(lim.buffers.local.stack_top == NULL)
		stack = lim.buffers.root.for_loop_stack_top;
	else
		stack = lim.buffers.local.stack_top->for_loop_stack_top;


	if(stack == NULL || cur_layer > stack->layer_base)
		return;

	free( NICK_CURRENT(nick_for_loop) );
	NICK_CURRENT(nick_for_loop) = stack->save_state;


	if(lim.buffers.local.stack_top == NULL)
		lim.buffers.root.for_loop_stack_top = stack->below;
	else
		lim.buffers.local.stack_top->for_loop_stack_top = stack->below;


	qee_free_queue(stack->idents);
	free(stack);
}


void new_local_environment(bool is_method){
	Func_Env_Stack *new;
	(lim.buffers.local.env_quant)++;

	new = malloc(sizeof(Func_Env_Stack));
	new->content        = tmpfile();
	new->local_func     = NULL;
	new->local_var_tab  = NULL;
	new->parameter      = NULL;
	new->is_method      = is_method;
	new->below          = NULL;

	#define NICK_TO_SELF(buf)                                              \
		if(new->is_method){                                                \
			if(buf == NULL)                                                \
				buf = qee_create("self", NULL, "Sa", false);               \
			else                                                           \
				qee_add_item(&(buf), "self", NULL, "Sa", false, QEE_DROP); \
		}

	if(lim.buffers.local.stack_top == NULL){
		lim.buffers.local.stack_top = new;

		NICK_TO_SELF(new->local_var_tab)
		return;
	}

	new->below = lim.buffers.local.stack_top;
	lim.buffers.local.stack_top = new;
	NICK_TO_SELF(new->local_var_tab)
}

void drop_local_environment(void){
	if(lim.buffers.local.env_quant > 0)
		(lim.buffers.local.env_quant)--;

	char c = 0;
	FILE *dest;
	Func_Env_Stack *top;


	top = lim.buffers.local.stack_top;
	lim.buffers.local.stack_top = top->below;

	dest = ((lim.buffers.local.stack_top == NULL) ? lim.buffers.destine_file : lim.buffers.local.stack_top->content);


	fseek(top->content, 0, SEEK_SET);

	while( (c = fgetc(top->content)) != EOF ){
		fputc(c, dest);

		if(top->is_method && c == ')'){
			top->is_method = false;
			fprintf(dest, "local Sa=self;");
		}
	}


	fclose(top->content);
	
	qee_free_queue(top->local_func);
	qee_free_queue(top->local_var_tab);
	qee_free_queue(top->parameter);

	For_Loop_Stack *cur, *below;
	for(cur = top->for_loop_stack_top; cur != NULL; cur = below){
		qee_free_queue(cur->idents);
		free(cur->save_state);

		below = cur->below;
		free(cur);
	}

	free(top);
}


char* save_ident_in_buffer(char *ident, char *table_key, bool is_root, NICK_ID id, Queue **buf){
	// get a new nickname to "ident"
	char *nick_tmp, **nick_buf;

	if(ident[0] != '_'){
		switch(id){
			case NICK_IDENT:    nick_buf = ((is_root) ? nick_global_ident : nick_local_ident); break;
			case NICK_STD_HDR:  nick_buf = nick_std_hdr;   break;
			case NICK_PARAM:    nick_buf = nick_parameter; break;
		}
		nick_tmp = get_and_update_nick(nick_buf);

	// started with '_' will not be compacted
	}else if(strcmp(ident, "_") == 0){
		nick_tmp = "__";
		
	}else{
		nick_tmp = ident;
	}


	if(id == NICK_STD_HDR){
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

char* save_lib_func_in_buffer(char *ident){
	free(nick_lib_func);
	nick_lib_func = malloc(strlen(ident) + 3);

	strcpy(nick_lib_func, "_.");
	strcat(nick_lib_func, ident);

	if(lim.buffers.root.lib_func == NULL)
		lim.buffers.root.lib_func = qee_create(ident, NULL, nick_lib_func, false);
	else
		qee_add_item(&(lim.buffers.root.lib_func), ident, NULL, nick_lib_func, false, QEE_DROP);

	return nick_lib_func;
}

char* save_for_loop_ident_in_buffer(char *ident){
	For_Loop_Stack *buf;
	char *nick;

	if(lim.buffers.local.stack_top == NULL)
		buf = lim.buffers.root.for_loop_stack_top;
	else
		buf = lim.buffers.local.stack_top->for_loop_stack_top;

	nick = get_and_update_nick(nick_for_loop);

	if(buf == NULL)
		buf->idents = qee_create(ident, NULL, nick, false);
	else
		qee_add_item(&(buf->idents), ident, NULL, nick, false, QEE_DROP);

	return nick;
}


char* get_nickname_of(char *ident, bool is_root){
	Queue *cur, *bufs[7];
	For_Loop_Stack *for_loop;

	if(ident[0] == '_')
		return ident;


	if(!is_root){
		Func_Env_Stack *env;

		for(env = lim.buffers.local.stack_top; env != NULL; env = env->below){
			bufs[0] = env->local_func;
			bufs[1] = env->local_var_tab;
			bufs[2] = env->parameter;

			for(short i = 0; i < 3; i++)
				if( (cur = qee_get_item(bufs[i], ident)) != NULL )
					return cur->nick;
			
			for(for_loop = env->for_loop_stack_top; for_loop != NULL; for_loop = for_loop->below)
				if( (cur = qee_get_item(for_loop->idents, ident)) != NULL )
					return cur->nick;
		}
	}

	const bool is_for_loop_stack_empty = (lim.buffers.root.for_loop_stack_top == NULL);
	const short max = (is_for_loop_stack_empty) ? 6 : 7;

	bufs[0] = lim.buffers.root.global_func;
	bufs[1] = lim.buffers.root.global_var_tab;

	bufs[2] = lim.buffers.root.func_from_lua;
	bufs[3] = lim.buffers.root.table_from_lua;
	
	bufs[4] = lim.buffers.root.func_from_header;
	bufs[5] = lim.buffers.root.table_from_header;
	
	if(!is_for_loop_stack_empty)
		bufs[6] = lim.buffers.root.for_loop_stack_top->idents;

	for(short i = 0; i < max; i++)
		if( (cur = qee_get_item(bufs[i], ident)) != NULL )
			return cur->nick;

	for(for_loop = lim.buffers.root.for_loop_stack_top; for_loop != NULL; for_loop = for_loop->below)
		if( (cur = qee_get_item(for_loop->idents, ident)) != NULL )
			return cur->nick;


	return ident;
}
