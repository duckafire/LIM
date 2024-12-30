#ifndef CORE_TOOLS_STRING_PLUS
#define CORE_TOOLS_STRING_PLUS

#include <stdbool.h>

typedef enum{
	STR_END,
	STR_START,
	STR_RESTART,
}STR;

void string_set(char **str, STR mode);
void string_add(char **str, char c);
char* string_copy(char *str);
bool string_compare(char *str0, char *str1);
int string_length(char *str);

#endif
