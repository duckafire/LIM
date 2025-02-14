#ifndef CORE_TOOLS_READ_HEADER_LIM
#define CORE_TOOLS_READ_HEADER_LIM

#include <stdio.h>
#include <stdbool.h>
#include "queue.h"

typedef enum{
	HF_READING_DISABLE,
	HF_NOT_FOUND,
	HF_CONTENT_READED,
}HF_OUT_STATUS;

typedef enum{
	HF_EOF_FOUND,
	HF_EMPTY_PART,
	HF_NEXT_PART,
}HF_READ_STATUS;

HF_OUT_STATUS read_header_file(char **indiv_part_status);
static bool check_partitions_separator(void);
static bool is_empty_file(FILE **file);
static bool is_empty_list(Queue **list);
static HF_READ_STATUS refine_brute_status(FILE **buf, Queue **list, bool new_part_found, bool isfile);
static void start_reading(HF_READ_STATUS *refined_status);
static bool read_top_header(void);
static bool read_code_scope(void);
static bool read_list(Queue **buf, bool is_this_found);

#endif
