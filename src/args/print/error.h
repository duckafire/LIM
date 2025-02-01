#ifndef ARGS_PRINT_ERROR
#define ARGS_PRINT_ERROR

#include <stdio.h>
#include <stdlib.h>

#define LIM_ERROR(code, msg, ...) \
	printf(msg, ##__VA_ARGS__);   \
	exit(code)

#define ERROR_source_file_not_exist(sourceFileName) \
	LIM_ERROR(1, "<LIM> The file \"%s\" not exist.\n\n", sourceFileName)

#define ERROR_flag_repetititon(flag) \
	LIM_ERROR(2, "<LIM> Repetition of the flag: \"%s\".\n\n", flag)

#define ERROR_suffix_expected_after_flag(flag) \
	LIM_ERROR(3, "<LIM> Suffix expected after flag: \"%s\".\n\n", flag)

#define ERROR_invalid_suffix_to_flag(flag, expected, provided) \
	LIM_ERROR(4, "<LIM> Invalid suffix to flag: \"%s\"\n\tExpected \"%s\", instead \"%s\".\n\n", flag, expected, provided)

#define ERROR_invalid_argument_to_flag_help(arg) \
	LIM_ERROR(5, "<LIM> Invalid argument to \"help\" flag: \"%s\".\n\n", arg)

#define ERROR_file_name_expected_instead_flag(flag) \
	LIM_ERROR(6, "<LIM> File name expected, instead flag: \"%s\".\n\n", flag)

#define ERROR_invalid_flag(flag) \
	LIM_ERROR(7, "<LIM> Invalid flag: \"%s\".\n\n", flag)

#define ERROR_unexpected_file_name(name) \
	LIM_ERROR(8, "<LIM> Unexpected file name: \"%s\".\n\n", name)

#define ERROR_dest_file_already_exist(dest) \
	LIM_ERROR(9, "<LIM> The destine/output file already exist: \"%s\".\n\n", dest)

#define ERROR_invalid_file_extension(name) \
	LIM_ERROR(10, "<LIM> Invalid file extension: \"%s\"\n\tExpected \"*.lua\".\n\n", name)

#endif
