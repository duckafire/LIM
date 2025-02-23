#ifndef CORE_NICK_MAN
#define CORE_NICK_MAN

#include <stdbool.h>
#include "tools/queue.h"
#include "tools/lim-global-variables.h"

typedef enum{
	NICK_STD_HDR,
	NICK_IDENT,
	NICK_PARAM,
	NICK_FOR_LOOP,
	NICK_LIB_FUNC,
}NICK_ID;

void start_nickname_buffers(void);
static void start_nick_buf(char *nick_buf[]);

void save_nicknames_state(Local_Env *lenv);
void restart_nicknames_state(Local_Env *lenv);

static char* get_and_update_nick(char *nick_buf[]);
static void update_nick_current(char *nick_buf[], const int last_char);
char* save_ident(char *ident, char *table_key, NICK_ID id, Queue **buf);

static bool search_in_buffers(char *ident, short max, ...);
char* get_nickname_of(char *ident);

void free_nickname_buffers(void);
static void free_nick_buf(char *nick_buf[]);

#endif
