#ifndef CORE_TOOLS_STRING
#define CORE_TOOLS_STRING

typedef enum{
	STR_END,
	STR_START,
	STR_RESTART,
}STR;

void string_set(char **str, STR mode);
char* string_copy(char *str);
void string_add(char **str, char c);

#endif
