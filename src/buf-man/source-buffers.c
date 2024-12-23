#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "struct.h"
#include "methods.h"
#include "indexes.h"

static FromSrcRoot *new_src_env;
static FromSrcFunc *new_func_env, *i_func_env, *tmp_func_env;

#define SRCB_CREATE_CORE(b, M)   \
	for(short i = 0; i < M; i++) \
		b->bufs[i];              \
	b->next = NULL;              \
	return b

FromSrcRoot* srcb_create(void){
	new_src_env = malloc(sizeof(FromSrcRoot));

	SRCB_CREATE_CORE(new_src_env, FROMSRC_TOTAL_GLOBAL_BUFS);
}

static FromSrcFunc* srcb_create_func_env(char *funcName){
	new_func_env = malloc(sizeof(FromSrcFunc));

	new_func_env->name = malloc(strlen(funcName) + 1);
	strcpy(new_func_env->name, funcName);

	SRCB_CREATE_CORE(new_func_env, FROMSRC_TOTAL_LOCAL_BUFS);
}

void srcb_add_func_env(FromSrcRoot *global, char *funcName){
	new_func_env = srcb_create_func_env(funcName);

	if(global->next == NULL){
		global->next = new_func_env;
		return;
	}

	srcb_insert_func_env(global->next);
}

static void srcb_insert_func_env(FromSrcFunc *func){
	if(func->next == NULL){
		func->next = new_func_env;
		return;
	}

	srcb_insert_func_env(func->next);
}

void srcb_remove_func_env(FromSrcRoot *global, char *funcName){
	if(strcmp(funcName, global->next->name) == 0){
		tmp_func_env = global->next->next;

		return;
	}

	for(i_func_env = global->next->next; strcmp(funcName, i_func_env->name) != 0; i_func_env = i_func_env->next);

	srcb_free_func_env(i_func_env, &tmp_func_env);
}

void srcb_free_func_env(FromSrcFunc *func, FromSrcFunc **tmp){
	for(short i = 0; i < FROMSRC_TOTAL_LOCAL_BUFS; i++){
		fclose(func->bufs[i]);
		func->bufs[i] = NULL;
	}
	
	*tmp = func->next;
	free(func);
}

FILE* srcb_get_buf(FromSrcRoot *global, short bufId, char *funcName){
	srcb_convert_bufId(&bufId);

	if(IS_GLOBAL_CODE(bufId))
		return global->bufs[bufId];

	for(i_func_env = global->next; strcmp(funcName, i_func_env->name) != 0; i_func_env = i_func_env->next);

	return i_func_env->bufs[ FROMSRC_ADJ_LOCAL_CODE(bufId) ];
}

void srcb_fseek_set_all(FromSrcRoot *global){
	short i;

	for(i = 0; i < FROMSRC_TOTAL_GLOBAL_BUFS; i++)
		fseek(global->bufs[i], 0, SEEK_SET);

	for(i_func_env = global->next; i_func_env != NULL; i_func_env = i_func_env->next)
		for(i = 0; i < FROMSRC_TOTAL_LOCAL_BUFS; i++)
			fseek(i_func_env->bufs[i], 0, SEEK_SET);
}

static fromsrc_buf_id srcb_convert_bufId(short *bufId){
	// other "format" were created to
	// easily other process
	if(*bufId > FROMSRC_TOTAL_GLOBAL_BUFS + FROMSRC_TOTAL_LOCAL_BUFS)
		*bufId = FROMSRC_CONSTANT;
}

void srcb_write_in(FromSrcRoot *global, short bufId, char *content, char *funcName){
	fprintf( srcb_get_buf(global, bufId, funcName), "%s\n", content);
}

void srcb_write_in_order(FromSrcRoot *global, short content){
	fwrite(&content, sizeof(short), 1, global->bufs[FROMSRC_ORDER]);
}

bool srcb_get_order(FromSrcRoot *global, short *bufId){
	return (fread(bufId, sizeof(short), 1, global->bufs[FROMSRC_ORDER]) > 0);
}

void srcb_redo_last_getted(FromSrcRoot *global, short bufId, char *contentGetted, char *funcName){
	fseek(global->bufs[FROMSRC_ORDER], -sizeof(short), SEEK_CUR);
	fseek(srcb_get_buf(global, bufId, funcName), -strlen(contentGetted), SEEK_CUR);
}

void srcb_free(FromSrcRoot *global){
	for(i_func_env = global->next; i_func_env != NULL; i_func_env = tmp_func_env)
		srcb_free_func_env(i_func_env, &tmp_func_env);

	for(short i = 0; i < FROMSRC_TOTAL_GLOBAL_BUFS; i++){
		fclose(global->bufs[i]);
		global->bufs[i] = NULL;
	}

	free(global);
}

#undef SRCB_CREATE_CORE
