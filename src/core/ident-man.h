#ifndef CORE_IDENT_MAN
#define CORE_IDENT_MAN

#include <stdbool.h>
#include "tools/queue.h"

typedef enum{
	NICK_STD_HDR,
	NICK_IDENT,
	NICK_PARAM,
}NICK_ID;

typedef struct Stack_Nick_Memory{
	char *save;
	struct Stack_Nick_Memory *below;
}Stack_Nick_Memory;

void start_nickname_buffers(void);
void restart_local_parameter_nicknames(void);
void save_local_parameter_state(void);
static void restart_nickname_of(char *nick_buf[], Stack_Nick_Memory **mem);
static void save_nickname_of(char *nick_buf[], Stack_Nick_Memory **mem);
static void start_nick_buf(char *nick_buf[]);
static char* get_and_update_nick(char *nick_buf[]);
static void update_nick_current(char *nick_buf[], const int last_char);
static void drop_nicknames_env_to_for_loop(void);
void free_nickname_buffers(void);
static void free_nick_buf(char *nick_buf[]);
static void free_nick_mem_stack(Stack_Nick_Memory *mem);

void new_nicknames_env_to_for_loop(unsigned short layer_base);
void pop_nicknames_env_to_for_loop(unsigned short cur_layer);

void new_local_environment(bool is_method);
void drop_local_environment(void);

char* save_ident_in_buffer(char *ident, char *table_key, bool is_root, NICK_ID id, Queue **buf);
char* save_lib_func_in_buffer(char *ident);
char* save_for_loop_ident_in_buffer(char *ident);

char* get_nickname_of(char *ident, bool is_root);

#endif
