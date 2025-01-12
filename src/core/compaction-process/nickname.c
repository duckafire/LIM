#include <stdlib.h>
#include <string.h>
#include "nickname.h"

static char *nick, *nick_formated;
static char nick_first_char, nick_end_char;

static const char nick_format_list[4] = {
	NICK_PREFIX_CHAR_NONE,
	NICK_PREFIX_CHAR_GLOBAL,
	NICK_PREFIX_CHAR_LOCAL,
	NICK_PREFIX_CHAR_PARAMETER,
};

void nick_init(NICK_TO type){
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

char* nick_get(NICK_PREFIX id){
	free(nick_formated);
	nick_formated = malloc(strlen(nick) + 2);

	nick_formated[0] = nick_format_list[id];
	nick_formated[1] = '\0';
	strcat(nick_formated, nick);

	return nick_formated;
}

void nick_update(void){
	nick_update_str( strlen(nick) - 1 );
}

static void nick_update_str(const int last){
	if(last > -1){
		if(nick[last] == nick_end_char){
			nick[last] = nick_first_char;
			nick_update_str(last - 1);
			return;
		}

		nick[last]++;
		return;
	}

	char *buf;
	const unsigned short len = (strlen(nick));

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
