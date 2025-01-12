#ifndef CORE_TOOLS_NICKNAME
#define CORE_TOOLS_NICKNAME

typedef enum{
	NICK_TO_LUA_AND_HEADER,
	NICK_TO_SOURCE_IDENTS,
}NICK_TO;

typedef enum{
	NICK_PREFIX_NONE,
	NICK_PREFIX_GLOBAL,
	NICK_PREFIX_LOCAL,
	NICK_PREFIX_PARAMETER,
}NICK_PREFIX;

#define NICK_PREFIX_CHAR_NONE      '\0'
#define NICK_PREFIX_CHAR_GLOBAL    'G'
#define NICK_PREFIX_CHAR_LOCAL     'L'
#define NICK_PREFIX_CHAR_PARAMETER '\0'

void nick_init(NICK_TO type);
char* nick_get(NICK_PREFIX id);
void nick_update(void);
static void nick_update_str(const int last);
void nick_end(void);

#endif
