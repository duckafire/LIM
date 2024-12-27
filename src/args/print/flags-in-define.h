#ifndef ARGS_PRINT_INFO_FLAGS
#define ARGS_PRINT_INFO_FLAGS

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
