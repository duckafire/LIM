#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../debug-common.h"
#include "../../../src/core/tools/string-plus.h"
#include "../../../src/core/tools/nickname.h"


static void string_plus_c(void);
static void nickname_c(void);

static short max;


#define MAX max = 51 + rand() % 50


int main(int argc, char *argv[]){
	CHECK_ARG
	srand(time(NULL));

	if(strcmp(argv[1], "string-plus.c") == 0)
		string_plus_c();

	else if(strcmp(argv[1], "nickname.c") == 0)
		nickname_c();

	else{
		INVALID_ARG("string-plus.c\nnickname.c");
	}

	return 0;
}


static void string_plus_c(void){
#define PRINTF printf("%s\n\n", dinamic)

	MAX;
	char *dinamic;


	PUTS("Create a \"dinamic string\" and add random characteres to it.");

	string_set(&dinamic, STR_START);
	for(short i = 0; i < max; i++)
		string_add(&dinamic, ' ' + rand() % 95);

	PRINTF;


	PUTS("End string, copy \"Hello world!\" to it, restart and end it");

	string_set(&dinamic, STR_END);
	dinamic = string_copy("Hello world!");

	PRINTF;
	string_set(&dinamic, STR_RESTART);
	string_set(&dinamic, STR_END);


	PUTS("Get it length and compare it with \"Hello world!\" and `NULL`");

	printf("Length:   %d\n",   string_length(dinamic));
	printf("Compare0: %d\n",   string_compare(dinamic, "HELLO WORLD!"));
	printf("Compare1: %d\n\n", string_compare(dinamic, NULL));


#undef PRINTF
}

static void nickname_c(void){
#define PRINTF(text, init, get)        \
	PUTS(text);                        \
	nick_init(init);                   \
	MAX;                               \
	max *= 2 + rand() % 4;             \
	for(short i = 0; i < max; i++){    \
		printf("%s  ", nick_get(get)); \
		nick_update();                 \
	}                                  \
	puts("\n\n");                      \
	nick_end()
//#enddef

	PRINTF(
		"Nicknames to table and functions from Lua\n\tand function from \"header.lim\"",
		NICK_TO_LUA_AND_HEADER, NICK_PREFIX_NONE
	);

	PRINTF(
		"Nicknames to \"private global\" variables, tables and functions",
		NICK_TO_SOURCE_IDENTS, NICK_PREFIX_GLOBAL
	);

	PRINTF(
		"Nicknames to local (function environment) variables, tables and functions",
		NICK_TO_SOURCE_IDENTS, NICK_PREFIX_LOCAL
	);

	PRINTF(
		"Nicknames to ALL paramteres",
		NICK_TO_SOURCE_IDENTS, NICK_PREFIX_PARAMETER
	);

#undef PRINTF
}
