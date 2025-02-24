#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "tools/lim-global-variables.h"
#include "tools/queue.h"
#include "layer-env.h"
#include "nick-man.h"

static Local_Env *lenv;
static Local_Env_Type type;

static bool is_valid_open(char *kw, char *tk){
	// NOTE: this "(tk != NULL) == true" indicates that
	// the new layer will be a function environment,
	// this function will be called to check if it is
	// a method
	type = LET_NULL;

	if(kw == NULL){
		if(tk == NULL || strchr(tk, ':') == NULL)
			type = LET_FUNC;
		else
			type = LET_METHOD;

	}else{
		if(strcmp(kw, "repeat") == 0)
			type = LET_REPEAT;

		else if(strcmp(kw, "if") == 0 || strcmp(kw, "for") == 0 || strcmp(kw, "while") == 0)
			type = LET_OTHER;
	}

	return (type != LET_NULL);
}

static bool is_valid_close(char *kw){
	if(lim.env_buf.lenv_quant == 0)
		return false;

	if(lim.env_buf.lenv_stack_top->type == LET_REPEAT)
		return (strcmp(kw, "until") == 0);

	return (strcmp(kw, "end") == 0);
}

void add_layer_env(char *kw, char *tk){
	if(!is_valid_open(kw, tk))
		return;

	lenv = malloc(sizeof(Local_Env));
	lenv->content = tmpfile();
	lenv->var     = NULL;
	lenv->func    = NULL;
	lenv->special = NULL;
	lenv->type    = type;
	lenv->below   = NULL;

	save_nicknames_state(lenv);
	lim.env_buf.lenv_quant++;

	if(type == LET_METHOD)
		lenv->special = qee_create("self", NULL, "Sa", false);

	if(lim.env_buf.lenv_stack_top == NULL){
		lim.env_buf.lenv_stack_top = lenv;
		return;
	}

	lenv->below = lim.env_buf.lenv_stack_top;
	lim.env_buf.lenv_stack_top = lenv;
}

void pop_layer_env(char *kw, bool force){
	if(!force && !is_valid_close(kw))
		return;

	char c;
	FILE *dest;
	bool top_placed;


	lenv = lim.env_buf.lenv_stack_top;
	lim.env_buf.lenv_stack_top = lenv->below;

	restart_nicknames_state(lenv);
	lim.env_buf.lenv_quant--;
	top_placed = (lenv->type != LET_METHOD);


	if(lim.env_buf.lenv_quant == 0)
		dest = lim.env_buf.destine_file;
	else
		dest = lim.env_buf.lenv_stack_top->content;

	fseek(lenv->content, 0, SEEK_SET);
	while( (c = fgetc(lenv->content)) != EOF){
		fputc(c, dest);

		if(!top_placed && c == ')'){
			top_placed = true;
			fprintf(lenv->content, "local Sa=self;");
		}
	}


	fclose(lenv->content);

	qee_free_queue(lenv->var);
	qee_free_queue(lenv->func);
	qee_free_queue(lenv->special);

	free(lenv);
}

void finish_forgot_env(void){
	Local_Env *cur;

	while( (cur = lim.env_buf.lenv_stack_top) != NULL )
		pop_layer_env(NULL, true);
}
