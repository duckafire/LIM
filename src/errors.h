#ifndef LIM_ERRORS
#define LIM_ERRORS

#include <stdio.h>
#include <stdlib.h>

#define er_repeatFlag(arg, pos) \
	printf("<LIM> Invalid use (repetition) of the flag: \"%s\" (#%d).\n\n", arg, pos); \
	exit(1)

#define er_unexpectedFlag(arg, pos) \
	printf("<LIM> Unexpected use of the flag: \"%s\" (#%d).\n\n", arg, pos); \
	exit(2)

#define er_argExpected(requester) \
	printf("<LIM> Argument expected after flag: \"%s\".\n\n", requester); \
	exit(3)

#define er_invalidFlag(arg, pos) \
	printf("<LIM> Invalid flag: \"%s\" (#%d).\n\n", arg, pos); \
	exit(4)

#define er_filesNamesOverflow \
	puts("<LIM> Many source files were specified (max.: 1).\n\n"); \
	exit(5)

#define er_nameNotSpecified \
	puts("<LIM> Source file name was not specified.\n\n"); \
	exit(6)

#define er_nonExistentFile(name) \
	printf("<LIM> Non-existent file: \"%s\".\n\n", name); \
	exit(7)

#define er_fileAlreadyExistent(name) \
	printf("<LIM> File already existent: %s.\n\tTry use: -r, --replace\n\n", name); \
	exit(8)

#define er_invalidSuffixToFlag(requester, expected, gived) \
	printf("<LIM> Invalid suffix to flag: \"%s\"\n\tExpected \"%s\", instead \"%s\".\n\n", requester, expected, gived); \
	exit(9)

#define er_invalidArgToHelp(arg) \
	printf("<LIM> Invalid argument to \"help\" flag: \"%s\".\n\n", arg); \
	exit(10)

#endif
