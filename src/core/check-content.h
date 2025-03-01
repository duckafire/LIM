#ifndef CORE_CHECK_CONTENT
#define CORE_CHECK_CONTENT

// NOTE: Divided in:
// // constants
// // identifiers

///// CONSTANTS /////

#include <stdbool.h>

bool clear_white_spaces(char *c);

bool is_number(char c, char **tmp);
bool is_string(char c, char **tmp);
bool is_table_env(char c, char **tmp);

bool is_commentary(char c);
static bool is_commentary_block(char c);
static bool clear_single_line_commentary(char c);

void is_special_char(char c, char **tmp);

///// IDENTIFERS /////

#include "tools/queue.h"

// ChecK Ident Array InDex
typedef enum{
	CKIA_LUA_KW,
	CKIA_LUA_STD_FUNC,
	CKIA_LUA_STD_TABLE,
}CKIA_ID;

bool is_identifier(char c, char **tmp);
static bool is_from_lua(char *ident, CKIA_ID id);
static bool is_from_header(char *ident, Queue *buf);

#endif
