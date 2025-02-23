#ifndef CORE_LAYER_ENV
#define CORE_LAYER_ENV

#include <stdbool.h>

static bool is_valid_open(char *kw, char *tk);
static bool is_valid_close(char *kw);
void add_layer_env(char *kw, char *tk);
void pop_layer_env(char *kw);

#endif
