#ifndef STAGES_TOOLS
#define STAGES_TOOLS

enum{
	END,
	START,
};

enum{
	NICK_TO_LUA_AND_HEADER,
	NICK_TO_SOURCE_IDENTS,
	NICK_GLOBAL = 0,
	NICK_LOCAL,
	NICK_PARAMETER,
};

#define NICK_PREFIX_GLOBAL    'G'
#define NICK_PREFIX_LOCAL     'L'
#define NICK_PREFIX_PARAMETER '\0'

void string_set(char **str, short mode);
void string_add(char **str, char c);

void nick_init(short type);
char* nick_get(short mode);
static void nick_update(const int last);
void nick_end(void);

#endif
