#ifndef CORE_HEADER_FILE_READ
#define CORE_HEADER_FILE_READ

#include <stdio.h>
#include <stdbool.h>
#include "../buf-man/buf-man.h"
#include "out-status.h"

HF_OUT_STATUS read_header_file(char **indiv_part_status);
static bool check_partitions_separator(void);
static bool is_empty_file(FILE **file);
static HF_READ_STATUS refine_brute_status(FILE **buf, Queue *item, bool brute, bool isfile);
static void start_reading(HF_READ_STATUS *refined_status);
static bool read_top_header(void);
static bool read_code_scope(void);
static bool read_list(Queue **buf);

#endif
