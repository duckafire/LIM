#ifndef CORE_READ_HEADER_FILE
#define CORE_READ_HEADER_FILE

#include <stdbool.h>

typedef enum{
	GET_SUCCESS,
	GET_FAIL,
}header_use_state;

typedef enum{
	EMPTY,
	NEXT,
	NO_NEXT,
	EOF_FOUND,
	FINAL_SUCESS,
}header_partition_state;

void get_header_file_content(void);
static header_use_state is_enable_header_file(void);
static bool is_partition_separator(void);
static header_partition_state get_content_from_top_header(void);
static header_partition_state get_content_from_code_scope(void);
static header_partition_state get_content_from_funct_list(void);

#endif
