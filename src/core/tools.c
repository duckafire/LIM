#include <stdlib.h>
#include <string.h>
#include "tools.h"

static char *nick, *nick_formated;
static char nick_first_char, nick_end_char;
static const char nick_format_list[3] = {
	NICK_PREFIX_GLOBAL,
	NICK_PREFIX_LOCAL,
	NICK_PREFIX_PARAMETER,
};

void string_set(char **str, short mode){
	free(*str);

	// START
	if(mode){
		*str = malloc(sizeof(char));
		*str[0] = '\0';
	}
}

void string_add(char **str, char c){
	const unsigned short len = strlen(*str);
	char *buf;

	buf = *str;

	*str = malloc(len + 2);
	strcpy(*str, buf);

	*str[len] = c;
	*str[len + 1] = '\0';

	free(buf);
}


void nick_init(short type){
	nick = malloc(sizeof(char) * 2);
	nick_formated = NULL;

	if(type == NICK_TO_LUA_AND_HEADER){
		nick_first_char = 'A';
		nick_end_char   = 'Z';
	}else{
		nick_first_char = 'a';
		nick_end_char   = 'z';
	}
	
	nick[0] = nick_first_char;
	nick[1] = '\0';
}

char* nick_get(short mode){
	nick_update( strlen(nick) - 1 );

	free(nick_formated);
	nick_formated = malloc(strlen(nick) + 2);

	nick_formated[0] = nick_format_list[ mode ];
	nick_formated[1] = '\0';
	strcpy(nick_formated, nick);

	return nick_formated;
}

static void nick_update(const int last){
	if(last > -1){
		if(nick[last] == nick_end_char){
			nick[last] = nick_first_char;
			nick_update(last - 1);
			return;
		}

		nick[last]++;
		return;
	}

	char *buf;
	const unsigned short len = strlen(buf);

	buf = nick;
	nick = malloc(len + 2);

	strcpy(nick, buf);
	nick[len] = nick_first_char;
	nick[len + 1] = '\0';
}

void nick_end(void){
	free(nick);
	free(nick_formated);

	nick = NULL;
	nick_formated = NULL;
}
