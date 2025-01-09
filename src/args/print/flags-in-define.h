#ifndef ARGS_PRINT_FLAGS_IN_DEFINE
#define ARGS_PRINT_FLAGS_IN_DEFINE

#include "../../cmake-h/version.h"

#define no_arguments_to_lim              \
	MESSAGE(                             \
		"[LIM] - Lua Library Compactor", \
		" ",                             \
		"Try: \"lim --help\""            \
	)

#define print_current_version \
	MESSAGE(LIM_VERSION)

#endif
