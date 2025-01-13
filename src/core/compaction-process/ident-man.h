#ifndef CORE_COMPACTION_PROCESS_IDENT_MAN
#define CORE_COMPACTION_PROCESS_IDENT_MAN

static void start_nickname_buffers(void);
static void start_nick_buf(char *nick_buf[]);
static char* get_and_update_nick(char *nick_buf[]);
static void update_nick_current(char *nick_buf[], const int last_char);
static void free_nickname_buffers(void);
static void free_nick_buf(char *nick_buf[]);

#endif
