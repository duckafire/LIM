#ifndef INTERFACES_ARGS_ERROR
#define INTERFACES_ARGS_ERROR

#include <stdio.h>
#include <stdlib.h>

#define LIM_ERROR(code, msg, ...) \
	printf(msg, ##__VA_ARGS__);   \
	exit(code)

#define ERROR_source_file_not_exist(sourceFileName) \
	LIM_ERROR(1, "<LIM> The file \"%s\" not exist.\n\n", sourceFileName)

#define ERROR_flag_repetititon(flag) \
	LIM_ERROR(2, "<LIM> Repetition of the flag: \"%s\"\n\n", flag)

#define ERROR_suffix_expected_after_flag(flag) \
	LIM_ERROR(3, "<LIM> Suffix expected after flag: \"%s\"\n\n", flag)

#define ERROR_unvalid_suffix_to_flag(flag, expected, provided) \
	LIM_ERROR(4, "<LIM> Invalid suffix to flag: \"%s\"\n\tExpected \"%s\", instead \"%s\".\n\n", flag, expected, provided)

#define ERROR_invalid_argument_to_flag_help(arg) \
	LIM_ERROR(5, "<LIM> Invalid argument to \"help\" flag: \"%s\".\n\n", arg)

#endif
