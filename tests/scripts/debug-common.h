#ifndef DEBUF_TESTS
#define DEBUF_TESTS

#include <stdio.h>

#define WARN(str) printf("[!] %s.\n\n", str)

#define CHECK_PROGRAM_ARG_QUANTITY         \
	if(argc == 1){                         \
		puts("Argument not specified.\n"); \
		return 1;                          \
	}

#define INVALID_ARG_TO_MAIN(opt)                    \
	printf("Invalid argument.\nTry:\n%s\n\n", opt); \
	return 1

#endif
