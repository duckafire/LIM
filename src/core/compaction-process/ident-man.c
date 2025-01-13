#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../lim-global-variables.h"
//#include "../buf-man/buf-man.h"
#include "ident-man.h"

// TODO: fast test

// #0: nickname to lua standard and header identifiers;
// NICK_CURRENT, NICK_FORMAT, NICK_FIRST, NICK_LAST, NICK_PREFIX
static char *nick_std_hdr[5]      = {NULL, NULL, "A", "Z", "\0"};
static char *nick_global_ident[5] = {NULL, NULL, "a", "z", "G"};
static char *nick_local_ident[5]  = {NULL, NULL, "a", "z", "L"};
static char *nick_parameter[5]    = {NULL, NULL, "a", "z", "\0"};

static const short LEN_2C = sizeof(char) * 2;

#define NICK_CURRENT(n) (n[0])
#define NICK_FORMAT(n)  (n[1])
#define NICK_FIRSTC(n)  (n[2][0])
#define NICK_FIRST(n)   (n[2])
#define NICK_LASTC(n)   (n[3][0])
#define NICK_LAST(n)    (n[3])
#define NICK_PREFIX(n)  (n[4])

static void start_nickname_buffers(void){
	start_nick_buf(nick_std_hdr);
	start_nick_buf(nick_global_ident);
	start_nick_buf(nick_local_ident);
	start_nick_buf(nick_parameter);
}

static void start_nick_buf(char *nick_buf[]){
	NICK_CURRENT( nick_buf ) = malloc(LEN_2C);
	strcpy(NICK_CURRENT( nick_buf ), NICK_FIRST( nick_buf ));
}

static char* get_and_update_nick(char *nick_buf[]){
	free( NICK_FORMAT(nick_buf) );
	NICK_FORMAT(nick_buf) = malloc(strlen( NICK_CURRENT(nick_buf) ) + LEN_2C);

	strcpy( NICK_FORMAT(nick_buf), NICK_PREFIX(nick_buf) );
	strcat( NICK_FORMAT(nick_buf), NICK_CURRENT(nick_buf) );

	update_nick_current( nick_buf, strlen( NICK_CURRENT(nick_buf) ) - 1 );
	return NICK_FORMAT( nick_buf );
}

static void update_nick_current(char *nick_buf[], const int last_char){
	if(last_char > -1){
		if(NICK_CURRENT( nick_buf)[last_char] == NICK_LASTC(nick_buf)){
			NICK_CURRENT(nick_buf)[last_char] =  NICK_FIRSTC(nick_buf);
			update_nick_current(nick_buf, last_char - 1);
			return;
		}

		NICK_CURRENT( nick_buf )[last_char]++;
		return;
	}

	char *buf;
	const unsigned short len = strlen(NICK_CURRENT(nick_buf));

	buf = NICK_CURRENT(nick_buf);
	NICK_CURRENT(nick_buf) = malloc(len + 2);

	strcpy(NICK_CURRENT(nick_buf), buf);
	strcat(NICK_CURRENT(nick_buf), NICK_FIRST(nick_buf));
}

static void free_nickname_buffers(void){
	free_nick_buf(nick_std_hdr);
	free_nick_buf(nick_global_ident);
	free_nick_buf(nick_local_ident);
	free_nick_buf(nick_parameter);
}

static void free_nick_buf(char *nick_buf[]){
	free( NICK_CURRENT(nick_buf) );
	free( NICK_FORMAT(nick_buf) );
}
