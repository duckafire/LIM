#ifndef CORE_TOOLS_NICKNAME
#define CORE_TOOLS_NICKNAME

typedef enum{
	NICK_TO_LUA_AND_HEADER,
	NICK_TO_SOURCE_IDENTS,
}NICK_TO;

typedef enum{
	NICK_PREFIX_GLOBAL    = 'G',
	NICK_PREFIX_LOCAL     = 'L',
	NICK_PREFIX_PARAMETER = '\0',
}NICK_PREFIX;

void nick_init(NICK_TO type);
char* nick_get(NICK_PREFIX id);
void nick_update(void);
static void nick_update_str(const int last);
void nick_end(void);

#endif
