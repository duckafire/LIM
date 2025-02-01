#ifndef CORE_COMPACTION_PROCESS_IDENT_MAN
#define CORE_COMPACTION_PROCESS_IDENT_MAN

#include <stdbool.h>
#include "tools/queue.h"

typedef enum{
	SCOPE_STD_HDR,
	SCOPE_IDENT,
	SCOPE_PARAM,
}SCOPE_ID;
// NOTE: there is not a PARAMETER scope
// this is only to choose it nickname

void start_nickname_buffers(void);
void restart_local_parameter_nicknames(void);
static void restart_nickname_of(char *nick_buf[]);
void save_local_parameter_state(void);
static void save_state_of(char *nick_buf[]);
static void start_nick_buf(char *nick_buf[]);
static char* get_and_update_nick(char *nick_buf[]);
static void update_nick_current(char *nick_buf[], const int last_char);
void free_nickname_buffers(bool saveds_included);
static void free_nick_buf(char *nick_buf[], bool saveds_included);

void new_local_environment(bool is_method);
void drop_local_environment(char **anony_func_to_local_declare);
char* save_ident_in_buffer(char *ident, char *table_key, bool is_root, SCOPE_ID id, Queue **buf);
char* get_nickname_of(char *ident, bool is_root);

#endif
